!taskkill -f -im EXCEL.exe
clear
clc

%% This section: print out results and display any visualizations of attacks
connectivity = 1; % this ratio/percentage can be altered according to what level of autonomy market penetration rate to test for
flag_ba =1; % turn this on if we want bounded acceleration to be considered - this is extended newell car following model / BA Newell
dsrc_range= 300; %ASL application range (also where the induction loop will be placed from the end of road) - can be altered

%% Clear the file which will contain the initial trace of vehicles at first time step
% if(connectivity==1)
% fileIDEW = fopen('ASLRingRoadTraceEW_q.txt','at');
% fileIDNS = fopen('ASLRingRoadTraceNS_q.txt','at');
% else
% fileIDEW = fopen('RingRoadTraceEW_q.txt','at');
% fileIDNS = fopen('RingRoadTraceNS_q.txt','at');
% end
% fprintf(fileIDEW, "\n");
% fprintf(fileIDNS, "\n");
% fclose(fileIDEW);
% fclose(fileIDNS);

%% Fundamental Diagram Settings
vf=15.0; % free flow speed; 15 m/s = 33 miles per hour; aka u; aka speed limit in average traffic network?
kj=1/7; % jam density; vehicles per meter
w = 4.67; % shockwave speed; as function of critical density,
% jam density, and free flow m/s
% kc = w / (vf+w) k_j, capacity C = vf*kc
kc= w / (vf+w) * kj; % critical density; vehicles per meter
% we need to recompute the first and second critical density for the
% MFD. need to make sure they are not decimals?
C=vf*kc; % capacity (max flow rate); vehicles/second
%t_delta = 0.15; % seconds
%n_delta = 0.1; % ??
% follows the triangular fundamental diagram - phi(k) = min {uk, w(kj-k)}
tau = 1/(w*kj); % time gap - time it takes for rear of car in front
tau = 1.5;
disp('tau');
disp(tau);
% to pass a point and the front of next car to reach same point
rho = 1/kj; % jam spacing

%% signal settings
% once simulation is done, can just check the data to figure out m.
% then, based on m, we can create the actual MFD
T=60.0; % seconds ; T一个cycle的时长
L= 900.0; % meters, length of ring road
L_int = 10.0; % meters, length of intersection
Y = 6.0; % seconds - yellow/amber - should be calculated based on intersection time,
AR = 0; % seconds - all red
% Y should follow this function to remove dilemma zone: Y >= Lint / u + tRE + u/2b
gT1 = 0.5; % ratio in [0,1]
gT2 = 0.5;
GT = 24.0; % 23 seconds
T1 = gT1 * T; % seconds.
T2 = gT2 * T;
%tot_time = 10* 60 * 60; % total time of simulation in seconds; 10 hrs = 10 * 60m/1hr * 60s/1min  = 36000
tot_time = 1200.0/3600.0; % N hr * 60 min * 60 s where N = num of hrs ; 2 hrs = 120 min / 1 min -> 120 cycles? 
%tot_N = floor(tot_time / T); % total number of cycles for the simulation总共多少个cycle
tot_N = 20.0; 

%% Initialization
N = 1.0; % default value; can definitely be changed
n_delt = 1.0; % 20 = # of vehicles; -> array of 20 / .1 -> 200 ; then each one will be updated by increasing index by 1?
t_delt = tau  * n_delt; % 1.5 * 1.0 = 1.5 -> each time step is 1.5 s
tot_sim = tot_time*round(T/t_delt)*60;%总共的timestep数量 -> total simulation time = 2 hrs * (60(s/min)/(1.5s/step)) * 60min/hr -> 4,800 ->steps?
% Dilemma Zone:
%Region where vehicles cannot either safely stop before stop line nor proceed through intersection during Yellow and All-Red (Amber). This is the yellow-light dilemma zone.
%This dilemma zone can be removed when a condition is satisfied given speed limit u (same as free flow?), reaction time t_re and max breaking deceleration b.
%Yellow Time + All-Red = Y >= L_int / u + t_reaction + u/2b

%% Start-up Behavior
%a = 2; % bounded acceleration (m/s^2)
ba = 1;

%% Clearance Behavior
%b = 4; % max deceleration rate (m/s^2)
%t_rea = 1; %reaction time maybe random sample from
% uniform distribution of [0.7-3]s
aggress = 1;
% might need to set this to 1 to speed up the simulation

%% ATTACK MODELING VARIABLES AND PARAMETERS
attack_start = 1; % this can be set to a static number for a case study; we might need to loop through many of them
attack_duration = tot_sim; % this can also be set to a static number for a case study. we might need to loop through many of them
time_att_ratio = 1.0;%全部percentage of time that is attacked when it is valid attack duration
att_periodic = 0; % if attack is periodic; this will be set to 1 
att_period = (30/t_delt); % if attack is periodic; this is period
att_offset = (30/t_delt); % this is the amount of time steps between each periodic attack
veh_att_ratio = 0.4;

% Attack vector:t_dur, t_start (Note: t_start + t_dur == t_end),
% percentage of actually attacked Targeted
% Controllers/Vehicles/Sensors - related to budget
attack_vector = [attack_start, attack_duration, time_att_ratio, ...
    att_periodic, att_period, att_offset, veh_att_ratio]; 

% Attack Flag Indexes correspond to Model Variables:
% 1:tcomm (# \Delta t steps), 2:cars in front (arrival time), 3: pos_err (x(t)), 
% 4: v(t) (arrival time), 5: worst-case attack
% If all are value "0", then no attacks will occur, and normal simulation
% will happen instead
attack_flags = [3,0,0,0,0,0];
user_attack_vals = [-5,-21,60,2.5,1.0]; % user-specified values (only for deterministic)

% Attack Flags can take on values from 1-5 for variables 1-3 to specify how the attack
% model behaves on the targeted variable.
% 0 - No attack - original value
% 1 - Extreme RANDOM
% 2 - Stealthy RANDOM
% 3 - Moderate random 
% 4 - Deterministic Value - Specified in the attack functions
% 5 - Denial-Of-Service - value becomes 0
% 6 - Constraint: Limited Attack Window - Must follow a
% specified attack window because attacker is not able to do the
% attack another time.
% 7 - Constraint: Limited Knowledge (Conditional Distribution)
% higher chance for vehicle to not be attacked

%% Print out the attack settings
disp('The attack model flags are: ');
disp(attack_flags);
disp('Flags from left to right are: 1: communication delay, 2:vehicles in front, 3: position, 4: velocity, 5: vasl, 6: worst-case (only 0 or 1)');
disp('Flag values: 1: random: unrestrained, 2: random: stealthy, 3: random: medium,');
disp('4: deterministic: unrestrained, 5: DOS - max possible delay, ');
disp('6: delay - some possible delay, 7: limited budget');
disp('The attack model settings are: ');
disp(attack_vector);
fprintf('1: attack start, 2: attack period, 3: ratio of attacked time steps\n');
fprintf('4: attack time gap/offset, and 5: ratio of attacked vehicles per time step\n');
pause(1);%暂停两秒

%% Simulation
compute_nfd = 1; % flag to actually create the nfd创建NFD
avgflow_arr = [];
avgflow_cycle = [];
waitingtimes = []; % < 0.1
waitingtimes2 =[];  % < avg speed
waitingtimes3 =[];
timelosses=[];
avg_durations=[];

% Strings for Logging Purposes
% asl settings
asl_settings_all = [];
if flag_ba == 1
    settings_str = strcat('nd',num2str(n_delt,2),'_ag',num2str(aggress,2),...
        '_hr',num2str(tot_time,2),'_ba',num2str(ba),'_v2x',num2str(connectivity,2)); 
else
    settings_str = strcat('nd',num2str(n_delt,2),'_ag',num2str(aggress,2),...
        '_hr',num2str(tot_time,2),'_v2x',num2str(connectivity,2));
end

xlsfile='./testresults/nfd';

% attack settings
if (connectivity > 0.0  && (attack_flags(1) > 0 || attack_flags(2) > 0 || attack_flags(3) > 0 || ...
        attack_flags(4) > 0 || attack_flags(5) > 0 || attack_flags(6) > 0 ))
    
    if(attack_flags(4) > 0 ) % periodic attack
    attack_params_str = ...
        strcat('ts', num2str(attack_vector(1),2), ...
        ' d', num2str(attack_vector(2)*t_delt,4), ...
        ' ft', num2str(attack_vector(3),2), ...
        ' p', num2str(attack_vector(5)*t_delt,2), ...
        ' o',num2str(attack_vector(6)*t_delt,2), ...
        ' fn',num2str(attack_vector(7),2));
    else % aperiodic attack
    attack_params_str = ...
        strcat('ts', num2str(attack_vector(1),2), ...
        ' d', num2str(attack_vector(2)*t_delt,4), ...
        ' ft', num2str(attack_vector(3),2), ...
        ' fn',num2str(attack_vector(7),2));
    end
else
    attack_params_str = 'no_attack';
end

if(connectivity>0)
if(attack_flags(1)>0)
    xlsfile=strcat(xlsfile, '_delay',num2str(attack_flags(1)));
    if(attack_flags(1)==4)
        xlsfile=strcat(xlsfile, '_', num2str(user_attack_vals(1)));
    end
end
if(attack_flags(2)>0)
    xlsfile=strcat(xlsfile,'_cif',num2str(attack_flags(2)));
    if(attack_flags(2)==4)
        xlsfile=strcat(xlsfile, '_', num2str(user_attack_vals(2)));
    end
end
if(attack_flags(3)>0)
    xlsfile=strcat(xlsfile,'_x',num2str(attack_flags(3)));
    if(attack_flags(3)==4)
        xlsfile=strcat(xlsfile, '_', num2str(user_attack_vals(3)));
    end
end
if(attack_flags(4)>0)
    xlsfile=strcat(xlsfile,'_v',num2str(attack_flags(4)));
    if(attack_flags(4)==4)
        xlsfile=strcat(xlsfile, '_', num2str(user_attack_vals(4)));
    end
end
if(attack_flags(5)>0)
    xlsfile=strcat(xlsfile,'_vasl',num2str(attack_flags(5)));
    if(attack_flags(5)==4)
        xlsfile=strcat(xlsfile, '_', num2str(user_attack_vals(5)));
    end
end
if(attack_flags(6)>0)
    xlsfile=strcat(xlsfile,'_worstcase',num2str(attack_flags(6)));
end

end
xlsfile = strcat(xlsfile,'_', settings_str,'.xlsx');

xlsfile2='./testresults/pos_vel';
if(connectivity > 0.0)

if(attack_flags(1)>0)
    xlsfile2=strcat(xlsfile2, '_delay',num2str(attack_flags(1)));
    if(attack_flags(1)==4)
        xlsfile2=strcat(xlsfile2, '_', num2str(user_attack_vals(1)));
    end
end
if(attack_flags(2)>0)
    xlsfile2=strcat(xlsfile2,'_cif',num2str(attack_flags(2)));
    if(attack_flags(2)==4)
        xlsfile2=strcat(xlsfile2, '_', num2str(user_attack_vals(2)));
    end
end
if(attack_flags(3)>0)
    xlsfile2=strcat(xlsfile2,'_x',num2str(attack_flags(3)));
    if(attack_flags(3)==4)
        xlsfile2=strcat(xlsfile2, '_', num2str(user_attack_vals(3)));
    end
end
if(attack_flags(4)>0)
    xlsfile2=strcat(xlsfile2,'_v',num2str(attack_flags(4)));
    if(attack_flags(4)==4)
        xlsfile2=strcat(xlsfile2, '_', num2str(user_attack_vals(4)));
    end
end
if(attack_flags(5)>0)
    xlsfile2=strcat(xlsfile2,'_vasl',num2str(attack_flags(5)));
    if(attack_flags(6)==4)
        xlsfile2=strcat(xlsfile2, '_', num2str(user_attack_vals(5)));
    end
end
if(attack_flags(6)>0)
    xlsfile2=strcat(xlsfile2,'_worstcase',num2str(attack_flags(6)));
end

end

xlsfile2 = strcat(xlsfile2,'_', settings_str,'.xlsx');

%% INCORPORATE CORRESPONDING ATTACK MODELS AND COMPUTE NFD/WAITING TIME RATIO METRICS
fprintf('Simulating and creating the NFD\n');
i = 1;
% can attempt to optimize which critical densities to choose for the NFD since
% n_delt can be less than 1 and critical densities can be integers
% first compute L/u = theta1*sT; theta1 = j1 + a1; j1 =
% floor(L/uT)
theta1 = L / (vf*T);
j1 = floor(L/(vf*T));%取整
theta2 = L / (w*T);
j2 = floor(L/(w*T));
a1 = theta1 - j1;
a2 = theta2 - j2;
kc1 = (j1 + min((a1/gT1), 1)) / (j1 + a1);
kc1 = kc1 * gT1 * kc;
kc2 = kj - (((j2 + min((a2/gT1), 1))*(gT1* C/w)) / (j2 + a2));
kc1_dens = kc1*L;
kc2_dens = kc2*L;
% Debugging Info
fprintf('Yellow time is: %f\n', Y);
fprintf('Green time is: %f\n', GT);
fprintf('Cycle time is: %f\n', T);
fprintf('Init time is: %f\n', round(T/t_delt));
fprintf('Total simulation time is: %f hrs\n', tot_time);
if(flag_ba == 1)
    fprintf('Bounded Acceleration is: %f m/s^2\n', ba);
end
fprintf('n delta is %f and t delta is %f\n', n_delt, t_delt);
fprintf('Percentages of aggressive and nonaggressive cars are: %f and %f\n', aggress * 100, (1-aggress)*100);
fprintf('Percentage of connected cars is: %f\n', connectivity*100);
fprintf('Percentage of attacked time steps is: %f\n', ...
    time_att_ratio*100);
fprintf('Percentage of attacked connected cars per time step is: %f\n',...
    veh_att_ratio*100);
fprintf('kc1 is %f2 and kc2 is %f2\n', kc1, kc2);
% 0<= a1 < 1; compute L/w = theta2 * T; theta2 = j2 + a2; j2 = floor(L/wT);0 <= a2 < 1
% crit density k1: (j1 + min(a1/pi , 1)) / (j1 + a1) * pi * kc
% crit density k2: kj - (j2+min(a2/pi, 1))/(j2+a2)*pi*C/w
pos = [];
pos_ring = [];
vel = [];
cores = 12;
step = 4;

fprintf('Starting simulation...\n');
% vector of vehicle quantities that will be simulated in single ring road
N_arr = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120]; % 128=L/7 vehicles is maximum (jam density)
for i = 1:length(N_arr)
    N=N_arr(i);
    if(N ~= 0)
        % write the number of cars in files for initial positions trace.
        % useful for comparing this simulation results with results from
        % other simulations requiring vehicle traces
%         if(connectivity==1)
%         fileIDEW = fopen('ASLRingRoadTraceEW_q.txt','at');
%         fileIDNS = fopen('ASLRingRoadTraceNS_q.txt','at');
%         else
%         fileIDEW = fopen('RingRoadTraceEW_q.txt','at');
%         fileIDNS = fopen('RingRoadTraceNS_q.txt','at');
%         end

%         fprintf(fileIDEW, '\n ------------------\n Number of cars N is: %d\n -----------------------\n\n ',N);
%         fprintf(fileIDNS, '\n ------------------\n Number of cars N is: %d\n -----------------------\n\n ',N) ;
%         fclose(fileIDEW);
%         fclose(fileIDNS);
        
        fprintf('Number of vehicles N is: %f\n', N);
        fprintf('Current density is %f and current ratio of cars to kj is: %f\n\n', N/900, N/900*7);
        [waitingtime,waitingtime2,waitingtime3,timeloss,avg_duration, ...
            cumQ, cumQ_cycle, vel, pos, pos_ring] =...
            attack_impact_calculation_ASL(N,Y,AR,T,GT,vf,kj,L,...
            t_delt,n_delt,tau,rho,tot_time,ba,aggress, attack_flags,...
            attack_vector,user_attack_vals, connectivity,...
            flag_ba,dsrc_range);
        
        %% Various metrics that are all optional
        avgflow_arr = [avgflow_arr cumQ];
        avgflow_cycle = [avgflow_cycle cumQ_cycle];
        waitingtimes = [waitingtimes waitingtime];
        waitingtimes2 = [waitingtimes2 waitingtime2];
        waitingtimes3 =[waitingtimes3 waitingtime3];
        timelosses = [timelosses timeloss];
        avg_durations = [avg_durations avg_duration];
        
%         % Write pos/vel traces into xls file if desired for path trace
%         visualization
%         xlswrite(xlsfile2, ...
%             pos, strcat('veh_pos',num2str(N)));
%         xlswrite(xlsfile2, ...
%             pos_ring, strcat('veh_pos_ring', num2str(N)));
%         xlswrite(xlsfile2, ...
%             vel, strcat('veh_vel', num2str(N)));
        fprintf('Cumulative average network flow for N:%d is: %f\n', N,cumQ);
        fprintf('Ratio of flow to optimal flow for N:%d is; %f\n',N, cumQ/C);
        fprintf('Done...\n');
    else
        avgflow_arr = [avgflow_arr 0];
        avgflow_cycle = [avgflow_cycle 0];
        waitingtimes = [waitingtimes 0];
        waitingtimes2 = [waitingtimes2 0];
        waitingtimes3 = [waitingtimes3 0];
        timelosses = [timelosses 0];
        avg_durations=[avg_durations 0];
        fprintf('The cumulative average network flow is: %f2\n', 0);
    end
end

%% Write/log all the results into xls file
fprintf('Now writing into xls files...');
N_arr = [0 N_arr];
avgflow_arr = [0 avgflow_arr];
avgflow_cycle = [0 avgflow_cycle];
waitingtimes = [0 waitingtimes];
waitingtimes2 = [0 waitingtimes2];
waitingtimes3 = [0 waitingtimes3];
timelosses = [0 timelosses];
avg_durations=[0 avg_durations];
minlen = min(length(N_arr), length(avgflow_arr));

xlswrite(xlsfile, ...
    attack_flags, strcat('attack_flags')); % note the attack settings ; 'tcomm','arrival time','gps error','asl vel'
xlswrite(xlsfile, ...
    [cell({'communication delay (tcomm)'}),cell({'cars in front (vif)'}), ...
    cell({'gps/position (x)'}),cell({'velocity (v)'}),cell({'ASL velocity (v_asl)'})], ...
    strcat('attack_flags_names'));

xlswrite(xlsfile, attack_vector, strcat('attack_vector'));
xlswrite(xlsfile,[N_arr(1:minlen);avgflow_arr(1:minlen)/C; waitingtimes(1:minlen); ...
    waitingtimes(1:minlen)*100.0; avgflow_arr(1:minlen); ...
    avgflow_cycle(1:minlen); waitingtimes2(1:minlen);waitingtimes3(1:minlen);...
    timelosses(1:minlen); avg_durations(1:minlen)],attack_params_str);
xlswrite(xlsfile, [cell({'N'}); cell({'avg flow/C'});...
    cell({'waiting ratio (<0.1m/s)'}); cell({'waiting ratio percent (<0.1m/s)'});cell({'avg flow'}); ...
    cell({'avg flow of last cycle'}); cell({'ratio (<= v_avg m/s)'}); cell({'ratio (> v_avg m/s)'}); ...
    cell({'avg time loss'}); cell({'avg travel duration'})], 'metrics names');

%   xlswrite(strcat(xlsfile,'_ndelt_', num2str(n_delt),'_ba_',num2str(flag_ba), '.xlsx'), pos, strcat('veh_pos'));
%   xlswrite(strcat(xlsfile,'_ndelt_', num2str(n_delt),'_ba_',num2str(flag_ba), '.xlsx'), pos_ring, strcat('veh_pos_ring'));
%   xlswrite(strcat(xlsfile,'_ndelt_', num2str(n_delt),'_ba_',num2str(flag_ba), '.xlsx'), vel, strcat('veh_vel'));
%% If desired, can add additional plotting code here
subplot(2,1,1);
plot(avgflow_arr(1:minlen)/C*100); % normalized ; percentage
axis([1 minlen 0 50]);
subplot(2,1,2);
plot(waitingtimes(1:minlen)*100); % not normalized ; percentage
axis([1 minlen 0 100]);
fprintf('Done.\n');


