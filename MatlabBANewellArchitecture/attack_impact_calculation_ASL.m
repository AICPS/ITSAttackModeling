function [waitingtime,waitingtime2,waitingtime3,timeloss,...
    avg_duration,cumQ,cumQ_cycle,veh_vel,veh_pos,pos_ring] = ...
    attack_impact_calculation_ASL(N,Y,AR,T,GT,vf,kj,L,...
    t_delt,n_delt,tau,rho,total_time,ba,aggress, attack_flags, ...
    attack_vector,user_attack_vals, connectivity,...
    flag_ba,dsrc_range);

%% Simulation Settings for the current N
M = round(N/n_delt); % this will adapt to the delta N vehicle step parameter if necessary
veh_accel = ba; % assume constant for now加速度
k = N/L; % vehicles per meter - constant since M is constant密度
cumQ = 0; % cumulative average network flow
cycle_time = round(T/t_delt);
init_time = 10*cycle_time;% if desired, this parameter may be used in the impact calculation (ignore initial time)
green_time = round(GT/t_delt);
yellow_time = round((Y+AR)/t_delt);
allred_time = round((AR/t_delt));
sim_time = round(total_time*cycle_time*60);
fprintf('Simulation time is...%f and with t_delt...%f\n', sim_time*t_delt,  sim_time);
fprintf('Init time is...%f and with t_delt...%f\n', init_time*t_delt, init_time);
fprintf('Cycle time is... %f, with t_delt...%f\n', T, cycle_time);
veh_pos = zeros(M, sim_time);
veh_vel = zeros(M, sim_time);
tcomm_mean_dsrc = 0.1; % 0.1 seconds mean for exponential distribution - dsrc
tcomm_mean_cell = 0.5; % 0.5 seconds mean for exponential distribution - cellular 

%% Assign connectivity to vehicles randomly (but ensure connectivity ratio is maintained)
%all cars corresponding to current car
isConnected = zeros(1,M);%keeps track if a vehicle is a connected vehicle or not;
for i = 1:M % need to improve this since it is random
    % determine if this should be connected/autonomous or not
    x=rand;
    if x<=(connectivity) && connectivity ~= 0 && length(find(i))/M < connectivity
        isConnected(i) = 1; % connected
    elseif(length(find(i))/M >= connectivity) % we reached the connect ratio
        break;
    end
end

% On a loop intersection, the positions of the vehicles at the start of the simulation are random based on jam spacing. 
s = rng;
r = normrnd(rho*n_delt,rho*n_delt,[1,M]);
sep = r; % separation
i = 2;
d_jam = 1/kj; %minimum distance required between each vehicle
tem_veh_pos = zeros(1,M);
while i <= M 
    %disp(i);
    sep(i) = max([rho*n_delt, sep(i)]);
    if(k>=(.5*kj) && rho*n_delt < sep(i))
        sep(i) = rho*n_delt;
    end
    % sep(i) = rho*n_delt;
    % %    tem_veh_pos(i) = max([0, max([tem_veh_pos(i-1) + sep(i), tem_veh_pos(i-1)+rho*n_delt])]);
    tem_veh_pos(i) = max([0, tem_veh_pos(i-1)+sep(i)]);
    %veh_vel(i,1) = vf;
    i = i + 1;
end
veh_pos(:,1) = sort(tem_veh_pos(1,:), 'descend')';
%  disp('Initial positions');
%  disp(veh_pos(:,1));
% % disp('Initial velocities');
% % disp(veh_vel(:,1));

dur = zeros(M,1); % contains the durations of each vehicle
i = M;

%% The following is code to print out the initial ring road vehicle
%% position trace for usage in SUMO and possibly other simulation tools
% if(connectivity==1)
% fileID1 = fopen('ASLRingRoadTraceEW_q.txt','at');
% fileID2 = fopen('ASLRingRoadTraceNS_q.txt','at');
% else
% fileID1 = fopen('RingRoadTraceEW_q.txt','at');
% fileID2 = fopen('RingRoadTraceNS_q.txt','at');
% end
% 
% while (i>=1)
%     fprintf(fileID1,...
%         '<vehicle id="%d" type="ASL_VTYPE" route="eastwest" depart="%d" departSpeed="%f" departPos="%f" color="1,1,0"/>\n',...
%          M - i, 0, 0.0, veh_pos(i,1));
%     fprintf(fileID2,...
%         '<vehicle id="%d" type="ASL_VTYPE" route="northsouth" depart="%d" departSpeed="%f" departPos="%f" color="1,1,0"/>\n',...
%          M - i, 0, 0.0, veh_pos(i,1));
%     i=i-1;
% end
% 
% fclose(fileID1);
% fclose(fileID2);

%% Provide flag to each vehicle if vehicle is aggressive or not
flag_agg = [];
i = 1;
while( i <= M )%遍历所有车辆看是否aggressive
    % for mixed aggressiveness
    x=rand;
    if round(x,1)<=(1-aggress)
        flag_agg=[flag_agg 0]; % non aggressive两个元素的向量，0代表non_agressive,1代表aggressive
    else
        flag_agg=[flag_agg 1]; % aggressive
    end
    i = i + 1;
end

% not used currently, but if the time step is less than the mean
% communication delay, then this distribution should be used to find out
% the delay 
pd = makedist('Gamma','a', 1, 'b', tcomm_mean_dsrc/t_delt);
t_pd = truncate(pd, tcomm_mean_dsrc/(2*t_delt), tcomm_mean_dsrc*2/t_delt);

% this variable will track the actual delays
tcomm_delay = zeros(1,M);%contains communication delay
% variable that will track the actual transmission times
tcomm_update = zeros(1,M);
max_velocities = zeros(1,M); %% to keep track of the last velocity given to this car between delays
leading_car = 1;

%% Simulation
asl_given = zeros(1,M); % this will keep track if a car has been provided an ASL speed or not (if connected)
vel_asl = zeros(1,M);
vel_asl(:) = vf;
t = 0;
t_sec = t_delt;
cnt = 2; % the time step counter
prev = 0; % the previous car index for the current vehicle
idx = 1;
% fprintf('init time is: %f\n', init_time);
% fprintf('total_time is: %f\n', round(total_time/t_delt));
ti_arr = zeros(1,M);
while cnt <= sim_time
    t = mod(cnt-1, cycle_time);
    t_sec = t_sec + t_delt;
    % determine leading car when light turns red
    % this changes for Bounded Acc
    if(flag_ba~=1 && t == (green_time + yellow_time))
        leading_car = findLeadingCar(veh_pos, leading_car, M,cnt,L);
    elseif(flag_ba==1 && t >= green_time && t <= (green_time + yellow_time))
        leading_car = findLeadingCar(veh_pos, leading_car, M,cnt,L);
    end
    
    %% Compute the new velocity and position
    % According to Newell model and ASL control algorithm; This can be
    % replaced with personal car-following model and ITS application.
    % However, you will need to copy over the attack models within
    % attack_ASL_calculation to your ITS application function.
    [veh_vel,veh_pos,asl_given,vel_asl,tcomm_delay,tcomm_update,ti_arr] =...
        attack_ASL_calculation(veh_pos,veh_vel,...
        L,cnt,tau,rho,n_delt,t_delt,t,green_time,yellow_time,cycle_time,...
        vf,tcomm_mean_dsrc,tcomm_delay,tcomm_update,t_pd,M,isConnected,...
        attack_flags,attack_vector,user_attack_vals,leading_car,asl_given,...
        vel_asl,ti_arr, t_sec,flag_ba,dsrc_range,ba);
    
    %fprintf('Leading car is: %d\n', leading_car);
    %fprintf('Current time in cycle = %f\n', t);
    %fprintf('Current average velocity is: %f\n', mean(veh_vel(:,1:cnt)));
    %fprintf('Time passed is %d\n', cnt);
    %cumQ = k * mean(mean(veh_vel(:,init_time:cnt)));
    cnt = cnt+1; % increment the time step
    if(mod(cnt-1, sim_time) == 0)
    %% For logging purposes and visualization, one may uncomment the 
    %% following to display the NFD, Positions, Velocities, Wait Times, etc.
    %% May need alteration for certain metrics. 
        %         fprintf('Current time cnt is: %d\n', cnt);
        %         subplot(2,1,1);
        %         plot(mod(veh_pos(:, :),900)');
        %         subplot(2,1,2);
        %         plot(veh_pos(:,:)');
        % %     elseif (mod(cnt-1, cycle_time)==0)
        % %         fprintf('cnt-1 is %d: Average network flow based on last cycle: %f, all cycles %f\n', cnt-1, k*mean(mean(veh_vel(:, cnt-cycle_time:cnt))),...
        % %            k*mean(mean(veh_vel(:, 1:cnt-1))) );
        % %        pause(2);
        %     elseif(mod(cnt-1, cycle_time*2)==0)
        %         veh_ring_pos = mod(veh_pos,L);
        %         subplot(2,1,1);
        %         plot(veh_ring_pos(:,cnt-cycle_time*2:cnt-1)');
        %         subplot(2,1,2);
        %         plot(veh_pos(:,cnt-cycle_time*2:cnt-1)');
        %         fprintf('cnt-1 is %d: Average network flow based on last cycle: %f, all cycles %f\n', cnt-1, k*mean(mean(veh_vel(:, cnt-1-cycle_time:cnt-1))),...
        %            k*mean(mean(veh_vel(:, min(cnt-1, init_time):cnt-1))) );
        %        fprintf('Average velocity is: %f\n', mean(mean(veh_vel(:, min(cnt-1,init_time):cnt-1))));
        %        pause(.1);
    end
end

%% TESTING: COMPUTING CUMULATIVE FLOW FROM JUST THE LAST CYCLE
     first_pos = veh_pos(:,init_time);
     last_pos = veh_pos(:,cnt-1);
     dist_covered = last_pos-first_pos;
     avg_dist_covered = mean(dist_covered);     
     avg_speed = avg_dist_covered/(t_delt*(cnt-1-init_time));
     avg_duration = (avg_speed/900);
    %     cumQ_dist = k * avg_speed;
    %     fprintf('Average metwork flow based on distance covered and optimized period is: %f\n',...
    %         cumQ_dist);
    %cumQ_cycle = k*mean(mean(veh_vel(:, cnt-1-cycle_time:cnt-1)));
    cumQ_cycle=0;
    % cumQ = k * mean(mean(veh_vel(:,init_time:cnt-1))); % if the user
    % wants to disregard some initial cycles before calculating stationary
    % flow
    cumQ = k * mean(mean(veh_vel(:,1:cnt-1)));

%timestring = strrep(datestr(datetime('now')),':','-');
%plot(k* mean(veh_vel(:,cnt-cycle_time*60:cnt-1))');
%saveas(gcf, strcat('./results/AttackDataNFD/Flows_N_',num2str(N),'_',timestring,'.png'));
%fprintf('Average network flow based on last cycle is: %f\n', cumQ_cycle);
%fprintf('Average network flow based on last 10 cycles: %f\n', k*mean(mean(veh_vel(:, cnt-1-10*cycle_time:cnt-1))));
fprintf('Average network flow based on all cycles from init is: %f\n', cumQ);

% size of array is MxSIMTIME M, sim_time
%% Below are several optional kinds of metrics that may be of interest to the user
% if the user wants to disregard some initial cycles before calculating stationary
% waiting time
% waitingtime = length(find(abs(veh_vel(:,init_time:cnt-1))<0.1))/(M*((cnt-1-init_time))); % M*sim_time
waitingtime = length(find(abs(veh_vel(:,1:cnt-1))<0.1))/(M*((cnt-1))); % M*sim_time
%waitingtime = waitingtime ;
%thresh=mean(mean(veh_vel(:, cnt-1-10*cycle_time:cnt-1)));
%waitingtime2 = length(find(abs(veh_vel(:,init_time:cnt-1))<=thresh))/(M*((cnt))); % M*sim_time
%waitingtime3 = length(find(abs(veh_vel(:,init_time:cnt-1))>thresh))/(M*((cnt))); % M*sim_time
%timeloss=length(find(abs(veh_vel(:,init_time:cnt-1))<vf))/(M*((cnt))); % M*sim_time
waitingtime2=0;
waitingtime3=0;
timeloss=0;
% for considering all of the time -> replace init_time with 1
% for considering only after initialized time -> replace 1 with init_time

fprintf('Average waiting time 1 (<0.1m/s) is: %f\n',waitingtime);
% fprintf('Average waiting time 2 (<= v_avg m/s) is: %f\n',waitingtime2);
% fprintf('Average waiting time 3 (> v_avg m/s) is: %f\n', waitingtime3);
% fprintf('Average time loss: %f\n', timeloss);
% fprintf('Average time duration: %f\n', avg_duration);
%     cumQ = k*mean(mean(veh_vel(:, cnt-1-cycle_time:cnt-1)));
pos_ring = mod(veh_pos,L);
%% Pictures of traces may be displayed and saved here; Change save directory if desired.
%timestring = strrep(datestr(datetime('now')),':','-');
%disp('Saving photo of vehicle positions...');
%     saveas(gcf, strcat('./_',num2str(N),'_',timestring,'.png'));
%disp('Done');
end

%% Function to find the leading car at the current time step "cnt"
function [leading_car] = findLeadingCar(veh_pos,leading_car,M,cnt,L)
      %if(t >= green_time && t <= green_time + yellow_time)
        %fprintf('at cnt-1 %d, current leader is car %d\n', cnt-1, leading_car);
        %        fprintf('Finding leader at time %d\n',t);
        allcurrposmods = mod(veh_pos(:,cnt-1),L);
        for i = 0:M-1
            if allcurrposmods(M-i) >= allcurrposmods(leading_car) && ...
                    allcurrposmods(M-i) <= 890
                %    fprintf('leading car updated to: %d\n', M-i);
                %    fprintf('Updating because: %f, %f\n ', allcurrposmods(M-i), allcurrposmods(leading_car));
                leading_car = M-i;
            elseif allcurrposmods(leading_car) > 890 && allcurrposmods(M-i) <= 890
                leading_car = M-i;
            end
        end
        %        fprintf('At this time, the leader is car %d\n', leading_car);
        %        fprintf('at cnt-1 %d, new leading car is car %d\n', cnt-1, leading_car);
end