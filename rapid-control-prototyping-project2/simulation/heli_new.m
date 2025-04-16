%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% heli.m
%% ACS61015 Matlab script to be run before Simulink files
%% Last revised: 21.05.2021
%% Matlab R2020b
%% Toolbox Dependencies;
%% Control Systems, Data Acquisition, Data acquisition toolbox support
%% package for National Instruments NI-DAQmx devices, Simulink Real-Time,
%% Aerospace Blockset, installation of Real-Time kernel.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

close all;  % close all figures
clear all;  % clear workspace variables
%daqreset;   % reset the DAQ device (error if not connected to DAQ)

%% Define model parameters
% Discrete Time MyDAQ Dynamics
ADC_Vres    = 20/((2^16)-1);    % ADC voltage resolution (V/bit)
Encoder_res = 2*pi/500;         % Encoder resolution (rad/wheel count)
DAC_Vres    = 20/((2^16)-1);    % DAC voltage resolution (V/bit)
DAC_lim_u   = 10;               % DAC upper saturation limit (V)
DAC_lim_l   = 0;                % DAC enforced lower saturation limit (V)
% Continuous Time Helicopter Dynamics
g   = 9.81;     % Gravitational acceleration (ms^-2) 
% Rigid body parameters
% Masses and lengths
m1 = 0.0505;   % mass of fan assembly (kg)
m2 = 0.100;    % mass of counterweight (kg)
l1 = 0.110;    % distance from helicopter arm to elevation axis (m);
l2 = 0.070;    % distance from fan centres to pitch axis (m);
l3 = 0.108;    % distance from counterweight to elevation axis (m);
% Inertias
Je = 2*m1*(l1^2)+m2*(l3^2);    % Inertia about elevation axis (kg*m^2);
Jt = Je;                       % Travel axis inertia
Jp = 2*m1*(l2^2);              % Pitch axis inertia

%% Ex 1: OPEN-LOOP TEST INTERFACE %%%%%%%%%%%%%
% Sensor calibration
PitchAxisData = readtable('PitchAxisData.txt');
ElevAxisData  = readtable('ElevAxisData.txt');
% Constraints
p_lim_u = max(PitchAxisData.Var1)*pi/180; % Upper pitch axis limit (rad)
p_lim_l = min(PitchAxisData.Var1)*pi/180; % Lower pitch axis limit (rad)
e_lim_u = max(ElevAxisData.Var1)*pi/180;  % Upper elevation axis limit (rad)
e_lim_l = min(ElevAxisData.Var1)*pi/180;  % Lower elevation axis limit (rad)
% Sample period
T = 0.02;   % Sample period (s)

%% Ex 2: MODELLING THE PITCH AXIS DYNAMICS %%%%%%%%%%%%%
% Pitch axis spring and damping constants
k_s = 0.0015;           % Spring constant (kg*m^2*s^-2)
k_d = 0.0005;           % Viscous damping (kg*m^2*s^-1)

%% Ex 3: MODELLING THE POWER ELECTRONICS %%%%%%%%%%%%%
% Power amplifier
k_a         = 1.2;   % Power amplifier voltage gain
amp_sat_u   = 12;   % Power amplifier upper saturation limit (V)
amp_sat_l   = 0;   % Power amplifier lower saturation limit (V)

%% Ex 4: MODELLING THE FAN DYNAMICS %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Table = readtable('lookup.txt');
ThrustMeas = Table(:,:);

% Fan voltage - thrust steady state behaviour
V_ab   = ThrustMeas.Var1;          % Fan voltage input (V)
Fss_ab = ThrustMeas.Var2;          % Steady-state fan thrust output (N)
% Fan voltage - thrust transient model.
tau = 0.6 ;             % 1st order time constant

%% Ex 5: DETERMINE EQUILIBRIUM CONTROL SIGNAL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Constant control input required to achieve hover
U_e = 3.8;           % Voltage output from myDAQ should be 6.0
U_a = U_e;
U_b = U_e;
%% Ex 6: DEFINE LTI STATE-SPACE CONTROL MODEL %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Approximate the fan voltage/thrust relationship by an affine           %
%  function of the form F_ab = alpha*V_ab+beta. Define alpha and beta.    %
alpha = 0.00459;
beta = -0.008  ;
plot(V_ab,Fss_ab,'kx');           % plot raw thrust data
grid on; hold on;
xlabel('Fan Voltage (V)');
ylabel('Output Thrust (N)');
plot(V_ab,alpha*V_ab+beta,'k-'); % plot linear approximation
%  State vector x:=[elev; pitch; trav; elev_dot; pitch_dot; trav_dot]     %
%  Note; these states model the dynamics of small perturbations around    %
%  the state of steady, level hover.                                      %
%  Define the control model given by x_dot = Ax + Bu, y = Cx + Du         %
A = [0, 0, 0, 1, 0, 0;
     0, 0, 0, 0, 1, 0;
     0, 0, 0, 0, 0, 1;
     0, 0, 0, 0, 0, 0;
     0, -k_s/Jp, 0, 0, -k_d/Jp, 0;
     0, (-2*l1*((alpha*k_a*U_e) + beta))/Jt, 0, 0, 0, 0;];

B = [0, 0;
     0, 0;
     0, 0;
     (l1*alpha*k_a)/Je, (l1*alpha*k_a)/Je;
     (l2*alpha*k_a)/Jp, (-l2*alpha*k_a)/Jp;
     0, 0;];

k_t = 0.01; % torque caused by fans

Br = [0, 0;
     0, 0;
     0, 0;
     (l1*alpha*k_a)/Je, (l1*alpha*k_a)/Je;
     ((l2*alpha*k_a)-(k_t*alpha*k_a))/Jp, ((-l2*alpha*k_a)-(k_t*alpha*k_a))/Jp;
     0, 0;];

C = [1, 0, 0, 0, 0, 0;
     0, 1, 0, 0, 0, 0;
     0, 0, 1, 0, 0, 0;];
D = zeros(3,2);

sysC = ss(A,B,C,D);


sysD = c2d(sysC,T,'zoh');

 %% Ex 7: Closed-loop simulation and test %%%%%%%%%%%%%%%%%%%%%%%%
% (This is where the development of your controller begins)

Cr = [1,0,0,0,0,0; % Augmented C matrix
      0,0,1,0,0,0]; % Selects the variables we are controlling

Aaug = [A zeros(6,2);   % Augmented A matrix
       -Cr zeros(2,2)]; 
Baug = [B;              % Augmented B matrix
        zeros(2,2)];

Qx = diag([10000 8000 5000 10000 1500 500 100 500]); % state cost matrix

 
Qu = 1*eye(2); % control cost matrix

Kaug = lqrd(Aaug,Baug,Qx,Qu,T); % Calculates optimal gains for system
                                % that minimise Qx and Qu

K = Kaug(:,1:6); % LQR Gain
ki = -Kaug(:,7:8); % Integral gain


sysC = ss(A,B,C,D); % Combine matrices into a state space model
sysD = c2d(sysC,T,'zoh'); % Discretise the state space model

Adt = sysD.A; % Extract each discretised Matrix
Bdt = sysD.B;
Cdt = sysD.C;
Ddt = sysD.D;

Gdt     = 1e-1*eye(6);
Hdt     = zeros(size(C,1),size(Gdt,2)); % No process noise on measurements

Rw = diag([1, 1, 1, 1, 1, 1]);
Rv = 1e-5*diag([1,1,1]);

sys4kf  = ss(Adt,[Bdt Gdt],Cdt,[Ddt Hdt],T);
[kdfilt, Ldt] = kalman(sys4kf,Rw,Rv)  ;   % Kalman filter synthesis

b = fir1(200, 0.004);