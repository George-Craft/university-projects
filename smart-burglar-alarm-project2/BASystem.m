clear all;
%Clear ports
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
arduino = serialport("/dev/cu.usbmodem1101",9600);
foreverIsOne=1;
pinExists=0; %pin does not exist at beginning
pause(2);
%variables
windowCheck = 0;
securityContactName = 'GeorgeCraft Security';
securityContactNumber = 45454545454;
User_PIN=0;
%service option table
options = {'Change Pin';'Add/Remove Face';'Change Transition Time';'Security Contact Details'; 'Check Status of Sensors'; 'Arm/Disarm System';'IDLE State'};
tb=table(['1';'2';'3';'4';'5';'6';'7'],options, 'VariableNames',{'Option Number' 'Option Service'});
clc;
input("Press Enter to Start");
write(arduino,1,"uint8");

while(foreverIsOne==1)
    clc;
    disp("SYSTEM IN IDLE STATE");
    disp("press ON button to start");disp(" ");
    constant=0;
    while (constant==0)
        arduinoMessage = read(arduino,1,"char");
        if(arduinoMessage=='B')
            constant=1; % to get out of loop
            disp("Button Pressed"); disp(" ");
            write(arduino,1,"uint8");
        else
            nothing=0; %to keep MATLAB busy and avoid warning messages of no reading
        end
    end
    flush(arduino);

    write(arduino,pinExists,"uint8"); %send arduino pin exists 0/1
    if(pinExists==0) %if pin does not exist
        [User_PIN, pinExists] = PINsetup(arduino);  %Go straight to PIN SETUP
        idleConstant=1; %go back to idle state

    elseif(pinExists==1) %LOGIN
        clc; disp("Login"); disp(" ");
        loginPass = login(arduino);
        if(loginPass==1)
            idleConstant=0; %go to service Options
        elseif(loginPass==2)
            idleConstant=1; %go to IDLE
        end

    end


    while(idleConstant==0)
        %service option display
        clc; disp(" ");disp(tb)
        service_option = input("Pick the service option: ");disp(" ");
        if (service_option == 1)
            disp(" ");disp("You chose option 1");disp(" ");
            pause(1);
            write(arduino,1,"uint8");
            [User_PIN] = changePIN(User_PIN, arduino);
            idleConstant = 1;
        elseif (service_option == 2)
            disp(" ");disp("You chose option 2");disp(" ");
            pause(1);
            write(arduino,2,"uint8");
            %Need function
        elseif (service_option == 3)
            disp(" ");disp("You chose option 3");disp(" ");
            write(arduino,3,"uint8");
            pause(1);
            transitionTime = getTransitionTime(arduino);
            [idleConstant,transitionTime] = TransitionTime(transitionTime,arduino);
        elseif (service_option == 4)
            disp(" ");disp("You chose option 4");disp(" ");
            pause(1);
            write(arduino,4,"uint8");
            [idleConstant,securityContactNumber,securityContactName] = Security_Details(securityContactNumber,securityContactName,arduino);
        elseif (service_option == 5)
            disp(" ");disp("You chose option 5");disp(" ");
            pause(1);
            write(arduino,5,"uint8");
            idleConstant = SenorStatus(arduino);
        elseif (service_option == 6)
            disp(" ");disp("You chose option 6");disp(" ");
            pause(1);
            write(arduino,6,"uint8");
            transitionTime = getTransitionTime(arduino);
            idleConstant = Arm_Disarm(transitionTime, User_PIN, arduino,securityContactName,securityContactNumber);
        elseif (service_option == 7)
            disp(" ");disp("You chose option 7");disp(" ");
            disp(" ");disp("Back to idle state");disp(" ");
            pause(1);
            write(arduino,7,"uint8");
            idleConstant=1;
            %needs function
        else
            disp(" ");disp("INVALID ENTRY.Back to idle state");disp(" ");
            pause(1);
            write(arduino,8,"uint8");
            idleConstant = 1;
        end

    end

end



%% PIN setup
function [User_PIN, pinExists] = PINsetup(arduino)
pinattempts=0;
while(pinattempts<2)
    clc;
    pinEntered = input("Enter new user pin: ");
    write(arduino,1,"uint8");
    %convert to array so PIN can be sent to arduino and to send the length
    [pinSender,enteredPinLength] = pinNumToArray(pinEntered);
    %check length size
    %send the length and wait for whether its the right length or not.
    write(arduino,enteredPinLength,"uint8");
    wait=0;
    while (wait==0)
        arduinoMessage = read(arduino,1,"char");
        if(arduinoMessage=='A')
            write(arduino,1,"uint8"); %disp("CORRECT LENGTH");
            wait=1;
            validLength=1;
        elseif(arduinoMessage=='B') %disp("ERROR! PIN LENGTH INVALID");
            write(arduino,1,"uint8");
            wait=1;
            validLength=0; pinVerified=0;
        else
            nothing=0;
        end
    end

    flush(arduino); %first flush the serial port

    %verify reocurring digits if length size is correct
    if(validLength==1)
        write(arduino,pinSender,"uint8"); %send the pin over
        wait=0;
        while(wait==0)
            arduinoMessage = read(arduino,1,"char");
            if(arduinoMessage=='A')
                write(arduino,1,"uint8");
                disp("PIN IS VALID");disp(" "); pause(2);
                pinVerified=1;
                wait=1;
            elseif(arduinoMessage == 'B')
                write(arduino,1,"uint8");
                disp("ERROR! PIN cannot contain one reoccuring digit");disp(" "); pause(2);
                pinVerified=0;
                wait=1;
            else
                nothing=0;
            end
        end
    end

    flush(arduino); %first flush the serial port

    if (pinVerified == 0)
        disp("Try Again!"); pause(2);
        pinattempts=pinattempts+1;
        if(pinattempts == 1)
            disp("Incorrect attempt, 1 attempt remaining"); pause(2);
        elseif(pinattempts == 2)
            disp("Too many incorrect attempts, Returning to IDLE");pause(2);
            pinExists = 0;
            User_PIN = 0;
        end
    elseif(pinVerified == 1)
        disp(['Your new pin is: ',num2str(pinEntered)]); pause(3);
        pinExists=1;
        User_PIN = pinEntered;
        pinattempts=9; %just to get out of the while loop
    end
end


end
%% PIN LOGIN
function loginValidate = login(arduino)
pinattempts=0; checkl=0; incorrect=0;
while(pinattempts<2)
    pinEntered = input("Enter user PIN: ");
    write(arduino,1,"uint8")
    %convert to array so PIN can be sent to arduino and to send the length
    [pinSender,enteredPinLength] = pinNumToArray(pinEntered);
    % send matlab pinlength
    write(arduino,enteredPinLength,"uint8");
    %check pin length size
    wait=0;
    while (wait==0)
        arduinoMessage = read(arduino,1,"char");
        if(arduinoMessage=='A') %disp("CORRECT LENGTH");
            write(arduino,1,"uint8");
            checkl=1;
            wait=1;
        elseif(arduinoMessage=='B')
            write(arduino,1,"uint8"); %disp("ERROR! PIN LENGTH INVALID");
            checkl=0;
            wait=1;
        else
            nothing=0;
        end
    end
    flush(arduino);

    if(checkl==1)
        %send the pin over
        write(arduino,pinSender,"uint8");
        %check pin digits
        wait=0;
        while(wait==0)
            arduinoMessage = read(arduino,1,"char");
            if(arduinoMessage=='A')
                write(arduino,1,"uint8");
                disp("PIN CORRECT"); disp(" ");pause(2);
                loginValidate = 1;
                pinattempts=9; %random value >2 to leave the loop
                wait=1;
                incorrect=0;
            elseif(arduinoMessage == 'B')
                write(arduino,1,"uint8");
                disp("PIN INCORRECT");disp(" ");pause(2);
                incorrect=1;
                wait=1;
            else
                nothing=0;
            end
        end
        flush(arduino);
    elseif(checkl==0)
        disp("PIN INCORRECT");disp(" ");pause(2);
        incorrect=1;
    end

    if(incorrect==1)
        loginValidate=2;
        pinattempts=pinattempts+1;
        if(pinattempts == 1)
            disp("Incorrect attempt, 1 attempt remaining"); disp(" ");pause(2);
        elseif(pinattempts == 2)
            disp("Too many incorrect attempts, Returning to IDLE");disp(" ");pause(2);
        end
    end


end
end

%% PIN CHANGE
function  [User_PIN] = changePIN(User_PIN, arduino)
pinattempts=0;
while(pinattempts<2)
    clc;
    disp("PIN CHANGE"); disp(" ");
    pinEntered = input("Enter new pin: ");
    write(arduino,1,"uint8");
    %convert to array so PIN can be sent to arduino and to send the length
    [pinSender,enteredPinLength] = pinNumToArray(pinEntered);
    %check length size
    %send the length and wait for whether its the right length or not.
    write(arduino,enteredPinLength,"uint8");
    wait=0;
    while (wait==0)
        arduinoMessage = read(arduino,1,"char");
        if(arduinoMessage=='A') %disp("CORRECT LENGTH");
            write(arduino,1,"uint8");
            wait=1;
            validLength=1;
        elseif(arduinoMessage=='B') % disp("ERROR! PIN LENGTH INVALID");
            write(arduino,1,"uint8");
            wait=1;
            validLength=0; pinVerified=0;
        else
            nothing=0;
        end

    end

    flush(arduino); %first flush the serial port

    %verify reocurring digits if length size correct
    if(validLength==1)
        write(arduino,pinSender,"uint8"); %send the pin over
        wait=0;
        while(wait==0)
            arduinoMessage = read(arduino,1,"char");
            if(arduinoMessage=='A')
                write(arduino,1,"uint8");
                disp("PIN IS VALID"); disp(" ");pause(2);
                pinVerified=1;
                wait=1;
            elseif(arduinoMessage == 'B')
                write(arduino,1,"uint8");
                disp("ERROR! PIN cannot contain one reoccuring digit");disp(" ");pause(2);
                pinVerified=0;
                wait=1;
            else
                nothing=0;
            end
        end
    end

    flush(arduino); %first flush the serial port

    if (pinVerified == 0)
        disp("Try Again!");
        pinattempts=pinattempts+1;
        if(pinattempts == 1)
            disp("Incorrect attempt, 1 attempt remaining"); disp(" ");pause(2);
        elseif(pinattempts == 2)
            disp("Too many incorrect attempts, Returning to IDLE");disp(" ");pause(2);
            pinChangeValidate = 0;
        end
    elseif(pinVerified == 1)
        disp(['Your new pin is: ',num2str(pinEntered)]); pause(3);
        pinChangeValidate=1;
        User_PIN = pinEntered;
        pinattempts=9; %just to get out of the while loop
    end
end


end

%%
%Transition Time
function [idleConstant,transitionTime] = TransitionTime(transitionTime,arduino)
clc;
disp(['Current transition time is: ',num2str(transitionTime)]); disp(" "); pause(2);

TTloop = 1;
while (TTloop == 1) % remain till transition time is set or two attempts completed
    userInput = input("Enter 1 to change current transition time or 2 to cancel: ");
    write(arduino,userInput,"uint8");
    if (userInput ==1)

        disp("");disp("Change transition time selected.");disp(" ");pause(2);

        timeAttempts=0;
        while(timeAttempts<2)
            numEntered = input("Enter new trasition time between 60 to 180 seconds: ");
            flush(arduino);
            write(arduino,1,"uint8");
            %convert to array so time can be sent to arduino and to send the length
            [numSender,enteredNumLength] = numToArray(numEntered);
            %check length size
            %send the length to arduino and wait for whether its the right length or not.
            write(arduino,enteredNumLength,"uint8");
            wait=0;
            while (wait==0)
                arduinoMessage = read(arduino,1,"char");
                if(arduinoMessage == 'X') %CORRECT LENGTH
                    write(arduino,1,"uint8");
                    wait=1;
                    validLength=1;
                elseif(arduinoMessage=='Y') %ERROR! LENGTH INVALID, TIME LENGTH HAS TO BE 2 OR 3
                    write(arduino,1,"uint8");
                    wait=1;
                    validLength=0; numVerified=0;
                else
                    nothing=0;
                end
            end


            flush(arduino); %first flush the serial port

            %verify 60<time<180 if length size correct
            if(validLength==1)
                write(arduino,numSender,"uint8"); %send the time over
                wait=0;
                while(wait==0)
                    arduinoMessage = read(arduino,2,"char");
                    if(arduinoMessage == 'X')
                        write(arduino,1,"uint8");
                        disp("TIME IS VALID");disp(" ");pause(2);
                        numVerified=1;
                        wait=1;
                    elseif(arduinoMessage == 'Y')
                        write(arduino,1,"uint8");
                        disp("ERROR! TIME HAS TO BE BETWEEN 60 AND 180 MINUTES");disp(" ");pause(2);
                        numVerified=0;
                        wait=1;
                    else
                        nothing=0;
                    end
                end
            end

            flush(arduino); %first flush the serial port

            if (numVerified == 0)
                timeAttempts=timeAttempts+1;
                if(timeAttempts == 1)
                    disp("Try Again!");
                    disp("Incorrect attempt, 1 attempt remaining"); disp(" ");pause(2);
                elseif(timeAttempts == 2)
                    disp("Too many incorrect attempts, Returning to IDLE");disp(" ");pause(2);
                    TTloop=0;
                    idleConstant=1; %go back to IDLE state in matlab
                end
                clc;
                disp("");disp("Change transition time selected.");disp(" ");
            elseif(numVerified == 1)
                clc;
                disp(['Your new transition time is: ',num2str(numEntered)]); pause(3);
                transitionTime = numEntered;
                timeAttempts=9; %just to get out of the while loop
                TTloop=0;
                idleConstant=1; %go back to IDLE state in matlab
            end
        end

        disp("end");

    elseif (userInput ==2)
        disp(" ");disp("Cancel selected.");disp(" ");pause(2);
        TTloop=0;
        idleConstant=1; %go back to IDLE state
    else
        disp(" ");disp("Wrong input.");disp(" ");pause(2);
        write(arduino,1,"uint8");
    end
end
end

%%
%Security Contact details
function [idleConstant,securityContactNumber,securityContactName] = Security_Details(securityContactNumber,securityContactName,arduino)
clc;
disp(" ");disp("Security Contact Detail is :");
disp(['Name: ', securityContactName]);
disp(['Number: +44(0)', num2str(securityContactNumber)]); disp(" ");
pause(3);
SCloop=1;
while SCloop==1
    clc;
    userInput = input("Enter 1 to change current security contact detail or 2 to cancel: ");
    write(arduino,userInput,"uint8");
    if (userInput ==1)

        disp(" ");disp("'Change security contact detail' selected.");disp(" ");
        pause(1);
        checkRange = 0;
        numAttempts=0;
        while (checkRange ==0)

            while(numAttempts<2)
                write(arduino,1,"uint8");
                clc;
                numEntered = input("Enter new security contact number: +44(0)");
                %convert to array so contact num can be sent to arduino and to send the length
                num = num2str(numEntered);
                sizE=size(num);
                enteredNumLength=sizE(2); %how many digits
                numSender = zeros(1,enteredNumLength);
                for i=1:enteredNumLength
                    numSender(i)= str2num(num(i));
                end
                %disp(["Your input is: ", num2str(numSender)]);disp(" ");pause(2);

                %check length size
                %send the length and wait for whether its the right length or not.
                write(arduino,enteredNumLength,"uint8");
                wait=0;
                while (wait==0)
                    arduinoMessage = read(arduino,2,"char");
                    if(arduinoMessage == 'X') %disp("CORRECT LENGTH");
                        write(arduino,1,"uint8");
                        wait=1;
                        validLength=1;
                        checkRange = 1;
                    elseif(arduinoMessage=='Y')
                        write(arduino,1,"uint8");
                        disp("ERROR! LENGTH INVALID, CONTACT NUMBER HAS TO BE 10 DIGITS");disp(" ");pause(2);
                        wait=1;
                        validLength=0;
                        numVerified=0;
                    else
                        nothing=0;
                    end
                end


                flush(arduino); %first flush the serial port

                %verify that the first digit is not '0' if length size correct
                if(validLength==1)
                    write(arduino,numSender,"uint8"); %send the time over
                    wait=0;
                    while(wait==0)
                        arduinoMessage = read(arduino,2,"char");
                        if(arduinoMessage == 'X')
                            write(arduino,1,"uint8");
                            disp("CONTACT NUMBER IS VALID");disp(" "); pause(2);
                            numVerified=1;
                            wait=1;
                        elseif(arduinoMessage == 'Y')
                            write(arduino,1,"uint8");
                            disp("ERROR! FIRST DIGIT OF CONTACT NUMBER SHOULD NOT BE 0");disp(" ");pause(2);
                            numVerified=0;
                            wait=1;
                        else
                            nothing=0;
                        end
                    end
                end

                flush(arduino); %first flush the serial port

                if (numVerified == 0)
                    disp("Try Again!");
                    numAttempts=numAttempts+1;
                    if(numAttempts == 1)
                        disp("Incorrect attempt, 1 attempt remaining"); disp(" ");pause(2);
                    elseif(numAttempts == 2)
                        disp("Too many incorrect attempts, Returning to IDLE"); disp(" ");pause(2);
                        SCloop=0;
                        idleConstant=1; %go back to IDLE state in matlab
                    end
                elseif(numVerified == 1)
                    disp(['Your Security Contact number is: ',num2str(numEntered)]); pause(3);
                    securityContactNumber = numEntered;
                    securityContactName = input("Enter new security contact name:  ", "s");
                    disp(" ");disp("Security contact detail changed to: ");disp(" ");
                    disp(['Name: ', securityContactName]);
                    disp(['Number: +44(0)', num2str(securityContactNumber)]); disp(" "); pause(5);
                    numAttempts=9; %just to get out of the while loop
                    SCloop=0;
                    idleConstant=1; %go back to IDLE state in matlab
                end
                flush(arduino);
            end

            disp("end");
        end
    elseif (userInput ==2)
        SCloop =0;
        idleConstant = 1;
        disp(" ");disp("Cancel selected. Back to IDLE.");disp(" ");pause(3);
    else
        disp(" ");disp("Wrong input.");disp(" ");pause(3);
        write(arduino,1,"uint8");
    end
end

end

%%
%sensor status - check status of sensors

function idleConstant = SenorStatus(arduino)

clc;buttonWait = 0;

MagStatus = "X";
PIRStatus = "X";
flush(arduino); %flush the port to ensure accurate readings
%check status of window
wait=0;
while(wait==0)
    MagSenStatus = read(arduino,1,"char");
    if (MagSenStatus == 'O')
        Magstatus = "OPEN";
        write(arduino,1,"uint8");
        wait=1;
    elseif (MagSenStatus == 'C')
        Magstatus = "CLOSED";
        write(arduino,1,"uint8");
        wait=1;
    else
        nothing=0;
    end
end
%check status of PIR sensor
wait=0;
while(wait==0)
    PIRSenStatus = read(arduino,1,"char");
    if (PIRSenStatus == 'O')
        PIRstatus = 'MOTION DETECTED';
        write(arduino,1,"uint8");
        wait=1;
    elseif (PIRSenStatus == 'C')
        PIRstatus = 'MOTION UNDETECTED';
        write(arduino,1,"uint8");
        wait=1;
    else
        nothing=0;
    end
end
%print table of sensors and their readings
options = [Magstatus; PIRstatus];
tb=table(['Window';'Motion'],options, 'VariableNames',{'Sensor' 'Status'});
disp(" ");disp(tb)
disp(" ");
disp("Press button to close...");

%service option table
options = {'Change Pin';'Add/Remove Face';'Change Transition Time';'Security Contact Details'; 'Check Status of Sensors'; 'Arm/Disarm System';'IDLE State'};
tb=table(['1';'2';'3';'4';'5';'6';'7'],options, 'VariableNames',{'Option Number' 'Option Service'});

%leave the table and go to idle when the button is pressed
while (buttonWait == 0)
    arduinoMessage = read(arduino,1,"char");
    if (arduinoMessage == 'B')
        buttonWait = 1;
        write(arduino,1,"uint8");
    else
        nothing=0;
    end
end

flush(arduino);
idleConstant = 1;

end

%%
function [idleConstant] = Arm_Disarm(transitionTime, User_PIN, arduino,securityContactName,securityContactNumber)
clc;
idleConstant = 0;
flush(arduino);
while (idleConstant == 0)
    disp("Verifying Sensors");disp(" ");
    pause(1);
    constant = 0;
    while (constant == 0)
        arduinoMessage = read(arduino,1,"char");

        if (arduinoMessage == 'C')
            disp("Window Closed"); disp(" ");pause(1);
            windowCheck = 1;
            constant = 1;
            write(arduino,1,"uint8");
        elseif (arduinoMessage == 'D')
            disp("Window Open"); disp(" ");pause(1);
            windowCheck = 2;
            constant = 1;
            write(arduino,1,"uint8");
        else
            nothing=0;
        end
    end
    write(arduino,1,"uint8");
    flush(arduino);

    if(windowCheck == 1)
        disp(" ");disp("All Sensors are Ready:)"); pause(1);
        %write(arduino,1,"uint8"); % we want arduino to continue on with arming system (orange led on etc)
        %display security contact details for 5 seconds
        disp(" ");disp("Security Contact Details:");  disp(" ");
        disp(['Name: ', securityContactName]);
        disp(['Number: +44(0)', num2str(securityContactNumber)]); disp(" ");pause(2);
        disp(" "); disp(['TransitionTime To leave the House: ', num2str(transitionTime)]);disp(" ");
        disp("ACTIVATING SYSTEM");disp(" "); pause(1);
        disp("STARTING COUNTDOWN");disp(" ");

        pause(1);
        for i=0:transitionTime
            disp(transitionTime - i);
            pause(1);
        end
        clc;
        write(arduino,1,"uint8");%tell arduino countdown finished
        %check if door is closed behind the user
        constant = 0;
        doorCheck = 0;
        onlyonce = 0;
        while (constant == 0)
            if (onlyonce == 0)
                disp("Checking If Door is Closed...");disp(" ");
                onlyonce=1;
            end
            arduinoMessageDoor = read(arduino,1,"char");
            if (arduinoMessageDoor == 'D')%Door Open
                write(arduino,1,"uint8");%Acknowledge 62
                constant = 1;
                doorCheck =1;
                disp("Failure to arm system - [Main door is not closed]");disp(" ");
                pause(1);
            elseif (arduinoMessageDoor == 'C')%Door closed
                write(arduino,1,"uint8");%Acknowledge 61
                constant = 1;
                doorCheck = 2;
                disp("Door closed as expected - User left the building.");disp(" ");
                pause(1);
            else
                nothing=0;
            end
        end
        %disp("come out of constant");
        %write(arduino,1,"uint8");%Update sensorCheck2
        flush(arduino);
        clc;
        constant2 = 0;
        while (constant2 == 0)
            if (doorCheck == 1)%door open sound buzzer to alert user that failure to arm system
                disp("Red Led ON"); disp(" ");pause(1);
                constant2 = 1;
            elseif (doorCheck == 2)
                disp("ARM Led ON"); disp(" ");pause(1);
                constant2 = 1;
                armSystem = 1;
            end
        end

        write(arduino,1,"uint8"); %line 92

        clc;
        if(doorCheck == 1)
            idleConstant = 1;
        else
            constant = 0;
            onetime = 0;
            wait = 0;
            one=0;
            if (armSystem == 1)
                while (constant ==0)
                    if (onetime == 0)
                        disp("System armed"); disp(" ");
                        pause(1);
                        disp("Armed state: checking sensors"); disp(" ");
                        onetime = 1;
                    end
                    while(wait == 0)
                        arduinoMessage = read(arduino,1,"char");
                        if (arduinoMessage == 'A')
                            write(arduino,1,"uint8");
                            sensorInfo=0;
                            while(sensorInfo==0)
                                arduinoMessageSensor = read(arduino,1,"char");
                                if(arduinoMessageSensor=='W')
                                    write(arduino,1,"uint8");
                                    disp("Window Sensor Triggered"); disp(" "); pause(1);
                                    sensorInfo=1;
                                elseif(arduinoMessageSensor=='P')
                                    write(arduino,1,"uint8");
                                    disp("PIR Sensor Triggered"); disp(" ");pause(1);
                                    sensorInfo=1;
                                end
                            end
                            flush(arduino);
                            disp("Potential Intruder");disp(" "); pause(1);
                            wait = 1;
                            constant = 1;
                        else
                            nothing=0;
                        end
                        if(one==0)
                            disp("ARMED STATE");
                            one=1;
                        end
                    end
                end
            end

            % add face check starts here



            % face check has ended


            %Start timer for PIN entry to be given. if not given
            %within the transtitionTime buzzer goes off.

            write(arduino,1,"uint8"); %sync with matlab for after face

            constantR=0;
            buttonPress=0;
            AlarmTrigger = 0;

            while(constantR==0)
                clc;
                enter=0;
                constantB=0;
                while(constantB==0)
                    if(enter==0)
                        disp("Press button to enter PIN");
                        enter=1;
                    end
                    arduinoMessage = read(arduino,1,"char");
                    if(arduinoMessage=='B')
                        buttonPress=1;
                        disp("Button Pressed");disp(" ");
                        write(arduino,1,"uint8"); %send 1 to get out of start loop
                        flush(arduino);
                    else
                        buttonPress=0;
                    end

                    if(buttonPress ==1) %enter pin
                        pinDisarmPass = checkPinDisarm(arduino);
                        flush(arduino);
                        if(pinDisarmPass==1)
                            %disarm
                            disp("System Disarmed - Back to Idle"); pause(2);
                            constantB=1;
                            idleConstant=1;
                            constantR = 1;
                            AlarmTrigger = 0;
                        else
                            constantB=1;
                        end
                    end
                    arduinoMessage = read(arduino,1,"char");
                    if(arduinoMessage == 'E')
                        write(arduino,1,"uint8");
                        disp("TIME IS UP! ALARM TRIGGERED");pause(2);
                        constantB=1;
                        constantR=1;
                        AlarmTrigger = 1;
                    elseif(arduinoMessage == 'F')
                        write(arduino,1,"uint8");
                        disp("5 ATTEMPTS EXCEEDED! ALARM TRIGGERED");pause(2);
                        constantB=1;
                        constantR=1;
                        AlarmTrigger = 1;
                    else
                        nothing=0;
                    end

                end
                %disp("left constantR"); pause(1);
            end
            %turn buzzer on as pin incorrect or time is up to enter pin
            %if goes past buzzer on limit turn of and disarm
            %if pin entered correct turn buzzer off and disarm

            if(AlarmTrigger ==1)
                clc;
                disp("WILL STOP AFTER 20 MINS IF NO PIN ADDED");disp(" ");pause(1);
                constantR2=0;
                while(constantR2==0)
                    enter=0;
                    constantB2=0;
                    while(constantB2==0)
                        if(enter==0)
                            disp("Press button to enter PIN to stop ALARM");
                            enter=1;
                        end
                        arduinoMessage = read(arduino,1,"char");
                        if(arduinoMessage=='B')
                            buttonPress=1;
                            disp("Button Pressed");disp(" ");
                            write(arduino,1,"uint8"); %send 1 to get out of start loop
                            flush(arduino);
                        else
                            buttonPress=0;
                        end

                        if(buttonPress ==1) %enter pin
                            pinDisarmPass = checkPinDisarm(arduino);
                            flush(arduino);
                            if(pinDisarmPass==1)
                                %disarm
                                disp("System Disarmed - Back to Idle"); pause(2);
                                constantB2=1;
                                constantR2=1;
                                idleConstant=1;
                            else
                                constantB2=1;
                            end

                        end
                        arduinoMessage = read(arduino,1,"char");
                        if(arduinoMessage == 'G') %if 20 mins is over or right pin entered
                            write(arduino,1,"uint8");
                            disp("got a G");
                            constantB2=1;
                            constantR2=1;
                            idleConstant=1;
                        else
                            nothing=0;
                        end

                    end
                end

            end


        end


    elseif(windowCheck == 2)
        disp(" ");disp("Sensors not ready :("); disp(" ");pause(2);
        disp("Going to IDLE");pause(2);
        idleConstant=1;
    end


    disp("idle constant end");
end
end

%%
function pinDisarmValidate = checkPinDisarm(arduino)
pinattempts=0; checkl=0; incorrect=0;

pinEntered = input("Enter user PIN: ");
write(arduino,1,"uint8") %leaves loop_until_one

%convert to array so PIN can be sent to arduino and to send the length
[pinSender,enteredPinLength] = pinNumToArray(pinEntered);
% send matlab pinlength
write(arduino,enteredPinLength,"uint8");
wait=0;
while (wait==0)
    arduinoMessage = read(arduino,1,"char");
    if(arduinoMessage == 'C')
        write(arduino,1,"uint8");
        disp("CORRECT LENGTH"); %bin after debugging
        checkl=1;
        wait=1;
    elseif(arduinoMessage=='D')
        write(arduino,1,"uint8");
        disp("ERROR! PIN LENGTH INVALID");disp(" ");
        checkl=0;
        wait=1;
    else
        nothing=0;
    end
end
flush(arduino);

if(checkl==1)
    %send the pin over
    write(arduino,pinSender,"uint8");
    wait=0;
    while(wait==0)
        arduinoMessage = read(arduino,1,"char");
        if(arduinoMessage == 'C')
            write(arduino,1,"uint8");
            disp("PIN CORRECT"); pause(1);
            pinDisarmValidate = 1;
            wait=1;
        elseif(arduinoMessage == 'D')
            write(arduino,1,"uint8");
            disp("PIN INCORRECT");pause(1);
            pinDisarmValidate = 2;
            wait=1;
        else
            nothing=0;
        end
    end
    flush(arduino);
elseif(checkl==0)
    disp("PIN INCORRECT"); pause(1);
    pinDisarmValidate = 2;
end
disp("i will leave checkpindisarm");

end

%% get transitionTime length
function [transitionTime]=getTransitionTime(arduino)
wait=0;
%disp("enter get transition time");
while(wait==0)
    transitionTimeLength=read(arduino,1,"char");
    if(transitionTimeLength=='2')
        %disp("entered");
        write(arduino,1,"uint8");
        transitionTimeLength=2;
        wait=1;
    elseif(transitionTimeLength=='3')
        %disp("entered");
        write(arduino,1,"uint8");
        transitionTimeLength=3;
        wait=1;
    end
    %disp("leave");
end
%disp("going to flush");
%disp(["length is",transitionTimeLength])
flush(arduino); %disp("finished flusg");
wait=0;
while(wait==0)
    %disp("r");
    arduinoMessage=read(arduino,1,"char");
    if(arduinoMessage=='R')
        nothing=0;
    else
        wait=1;
    end
end
wait=0;
if(transitionTimeLength==2)
    wait=0;
    while(wait==0)
        m=read(arduino,2,"uint8");
        disp("in 2");
        if(m>47)
            disp("in 2 <47");
            TTime=m-48;
            transitionTime=TTime(1)*10 + TTime(2);
            write(arduino,1,"uint8");
            wait=1;
        end
    end
elseif(transitionTimeLength==3)
    wait=0; k=0;
    disp("in 3");
    while(wait==0)
        m2=read(arduino,1,"uint8");
        m3=read(arduino,1,"uint8");
        if(k==0)
            m1=read(arduino,1,"uint8");
            k=1;
        end
        if(m1>47 && m2>47 && m3>47)
            disp("in 3 again");
            TTime=[m1-48,m2-48,m3-48];
            transitionTime=TTime(1)*100 + TTime(2)*10 +TTime(3);
            write(arduino,1,"uint8");
            wait=1;
        end
    end
end
% disp("transition time is: ");
% disp(transitionTime);pause(5);
end

%%
function[pinSender,enteredPinLength] = pinNumToArray(pinEntered)
if((pinEntered <1000000000) || (pinEntered >0) || (isempty(pinEntered)==0))
    pin = num2str(pinEntered);
    sizE=size(pin);
    enteredPinLength=sizE(2); %how many digits
    pinSender = zeros(1,enteredPinLength);
    for i=1:enteredPinLength
        pinSender(i)= str2num(pin(i));
    end
    %disp(["your entered pin is: ", num2str(pinSender)]);
else
    pinSender = [0];
    enteredPinLength=0;

end
end

%%
function[numSender,enteredNumLength] = numToArray(numEntered)
if((numEntered >1000000000) || (numEntered <0) || (isempty(numEntered)==1))
    numSender = [0];
    disp('BOZOED')
    enteredNumLength=0;
else
    num = num2str(numEntered);
    sizE=size(num);
    enteredNumLength=sizE(2); %how many digits
    numSender = zeros(1,enteredNumLength);
    for i=1:enteredNumLength
        numSender(i)= str2num(num(i));
    end
    %disp(["Your input is: ", num2str(numSender)]);
end
end

%%
function flush(arduino)
constant=0;
while(constant==0)
    arduinoMessage = read(arduino,1,"char");
    if(arduinoMessage=='R')
        constant=1;
        write(arduino,1,"uint8");
    end
end

end

