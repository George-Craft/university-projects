%% Toolboxes required to run this code: curve_fitting_toolbox , statistics_toolbox

clc
clear all % clear variables and command window

T = readtable('household_energy_data.csv'); % loads data into a table
T = removevars(T,["RadonLevel_Bqm3","windBearing","windSpeed","dewPoint","pressure"]); % remove the variables that wont have a noticable effect/any effect on power
Tcols = height(T); % retrieves the height of the table
%% Convert all data to numeric numbers


NewWeather = zeros(Tcols,1); % creates an empty variable NewWeather
for i = 1:1:Tcols  % creates a column vector with integer encoded data 
    if strcmpi(T.WeatherIcon(i,1) , 'snow')
        NewWeather(i,1) = -4;
    elseif strcmpi(T.WeatherIcon(i,1) , 'rain')
        NewWeather(i,1) = -3;
    elseif strcmpi(T.WeatherIcon(i,1) , 'fog')
        NewWeather(i,1) = -2;
    elseif strcmpi(T.WeatherIcon(i,1) , 'wind')
        NewWeather(i,1) = -1;
    elseif strcmpi(T.WeatherIcon(i,1) , 'cloudy')
        NewWeather(i,1) = 0;
    elseif strcmpi(T.WeatherIcon(i,1) , 'partly-cloudy-night')
        NewWeather(i,1) = 1;
    elseif strcmpi(T.WeatherIcon(i,1) , 'partly-cloudy-day')
        NewWeather(i,1) = 2;
    elseif strcmpi(T.WeatherIcon(i,1) , 'clear-night')
        NewWeather(i,1) = 3;
    elseif strcmpi(T.WeatherIcon(i,1) , 'clear-day')
        NewWeather(i,1) = 4;
    end
end

T = [T(:,1:24) , array2table(NewWeather)]; %combine original data with new numerical data
T.TotalKw = sum(T{:,2:17},2); % add new column which totals Kw usage

%% Handle missing data

T.Dishwasher_kW_(isinf(T.Dishwasher_kW_)) = NaN; % these two lines of code replace any inf scores with a NaN score
T.Kettle_kW_(isinf(T.Kettle_kW_)) = NaN;        
T = rmmissing(T); % this line of code removes any rows with a NaN Value


%% Normalize and remove outliers

T = normalize(T); % use standardization to scale and center the data

T = rmoutliers(T,'movmean',90); % removes outliers using moving mean method with a window size of 90 data points

T = removevars(T,["VentilationAndAC_kW_","CentralHeating_kW_","Dishwasher_kW_","UnderFloorHeater_kW_","ElectricRadiator_kW_","HomeOffice_kW_","Fridge_kW_","CellarSumpPump_kW_","GarageDoor_kW_","Oven_kW_","CeramicHob_kW_","Kettle_kW_","MechanicalLoftVentilator_kW_","ShowerPump_kW_","Microwave_kW_","LivingRoom_kW_"]);

%% check F-stat and P-value

removeCols = [];  %create an empty variable

for j = 2:width(T)  
    lm = fitlm(T{:, j}, T{:, 1}); % obtain 
    if lm.ModelFitVsNullModel.Fstat < 1 || lm.ModelFitVsNullModel.Pvalue > 0.05 %check if f stat and p avlue are acceptable
        removeCols(end+1) = j; % if not add to a list of columns to delete
    else
         
    end
end

T(:, removeCols) = [];  % Remove columns from table T

%% Plots of each individual variables

for i = 2:width(T)
    figure('units','normalized','outerposition',[0 0 1 1]) % create figures that are fullscreen
    for j = 1:9
        createFit(T{:,i},T.EnergyRequestedFromGrid_kW_,j,T.Properties.VariableNames{i}) %calls a function made using curve fitter that has been edited to feed in values
         
    end
end


%% PCA (most of this code is abstracted from lab 5)

 Y = T{:,1};
 X = T{:,2:end};
 C = X'*X; % compute square of data matrix
[P,L]=eig(C,'Vector'); %compute the eigendecomposition and return eigenvalues as a vector

[L_sort, idx]=sort(L, 'descend');

P_sort=P(:,idx);

Vq=pca(X);
Z = X*Vq; 

X_tilde=X*P_sort(:,1:end);

PSI=[ones(size(X_tilde,1),1) X_tilde]; %PSI is obtained via the PCA-reduced set of features
w_hat=PSI'*PSI\PSI'*Z;

PSI2=[ones(size(Y,1),1) Y]; %Psi2 is the classical regressor

w_hat2=PSI2'*PSI2\PSI2'*Z; 

fwdmod1=PSI*w_hat;
fwdmod2=PSI2*w_hat2;

L_sort_norm=L_sort/max(L_sort);
L_sort_norm_weighted=L_sort_norm/sum(L_sort_norm);
figure()
bar(L_sort_norm_weighted)
title("Relative contribution of each Eigenvector")
%% Build regression models  

figure('units','normalized','outerposition',[0 0 1 1]) % create figures that are fullscreen
    for j = 1:9
    createFit(fwdmod1,fwdmod2,j,'PCA') %calls a function made using curve fitter that has been edited to feed in values
    end
    sgtitle('Energy Requested from the grid Vs Variables') % titles each figure
