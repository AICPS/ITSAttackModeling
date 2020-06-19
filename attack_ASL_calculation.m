%% USED FOR SUMO TRACE WRITING
%last_car_idx = zeros(M,1);

%%   This function will update the maximum velocity of a connected
%%   vehicle according to the ASL control algorithm and incorporate attack
%%   models in the computation of said maximum velocity.
function [v,x,asl_given,vel_asl,tcomm_delay,tcomm_update,ti_arr] = ...
    attack_impact_calculation(x,v,L,cnt,tau,rho,...
    n_delt,t_delt,t,GT,Y,T,vf,tcomm_mean_dsrc,...
    tcomm_delay,tcomm_update,t_pd,M,isConnected,...
    attack_flags,attack_vector,user_attack_vals,...
    leading_car,asl_given,vel_asl,ti_arr, t_sec,flag_ba, ...
    dsrc_range,ba)

%% ATTACK MODELING PARAMETERS
tcomm_flag = -1;
cif_flag  = -1;
gps_flag = -1;
vel_flag = -1;
v_asl_flag = -1;
worstcase_flag = -1;
tcomm_flag = attack_flags(1);
cif_flag = attack_flags(2);
gps_flag = attack_flags(3);
vel_flag = attack_flags(4); % get the flags for each parameter
v_asl_flag = attack_flags(5);
worstcase_flag = attack_flags(6); % either on or off
comm_att = user_attack_vals(1);
cif_att = user_attack_vals(2);
gps_att = user_attack_vals(3);
vel_att = user_attack_vals(4);
vel_asl_att = user_attack_vals(5);

%% TIME RELATED PARAMETERS
g_time = 24;
y_time = 6;
T_time = 60;
L_int = 10;
comm_range =  L-dsrc_range;% assuming that controller is at end of intersection
maxCars=floor((dsrc_range)/(n_delt*rho))-1; %42
i = 1; % i and idx are used to iterate through the vehicles at this current time step
idx = leading_car;% start off from leading_car - this will be updated later
while i <= M% for every vehicle for the current time step, update their pos and spd
    if(idx==1) % car identification number
        prev = M; % car in front
    else
        prev = idx-1;% car in front
    end
    curr_pos = x(idx,cnt-1);%current veh position
    prev_pos = x(prev,cnt-1);%veh in front position
    mod_curr_pos = mod(curr_pos,L);% current veh ring position
    r = rand();
    
% %     fprintf('Current time is: %f2 \n', t_sec);
% %     fprintf('Vehicle %d has original arrival time of %f2 \n', idx, ti_arr(idx));
    %% When the car has not received Vasl and is in comm range
    if(isConnected(idx)==1 && mod_curr_pos < 890 ...
            && (mod_curr_pos) >= comm_range ...
            && asl_given(idx) == 0)
         % used for random chance to perform attacks on this car
        if(tcomm_update(idx) == 0) %% Compute tcomm (with/without attack)
            tcomm_sample=random(t_pd);% if the tstep is less than mean of communication delay, this can be used; for now, not used
            tcomm_delay(idx)= max([t_delt,tcomm_sample])/t_delt;
            tcomm_update(idx)=cnt+tcomm_delay(idx); % ACTUAL time that the traffic controller will send ASL
            alpha_tcomm = ...
                getAttComm(cnt,t_delt,tcomm_flag,...
                    tcomm_mean_dsrc,attack_vector,r,vf,...
                    g_time,y_time,L,L_int,comm_att);
            tcomm_delay(idx) = tcomm_delay(idx) + round((alpha_tcomm/t_delt)); 
            tcomm_delay(idx)=tcomm_delay(idx) + max([t_delt,tcomm_sample])/t_delt; 
            tcomm_update(idx)=cnt-1+tcomm_delay(idx); %delay update
        end
        %% The current time is now the time that the controller will compute ASL
        if(cnt-1==tcomm_update(idx)) % update with vasl and delayed vel and pos
            %% compute gps position error (with/without attack)
            % normal range of gps error is randomly between [-5,5]
            currtime = t_sec;
            %% TCOMM DELAY DETERMINING VELOCITY AND POSITION
            tcomm_sample=random(t_pd);
            delay_idx = max([t_delt,tcomm_sample])/t_delt; 
            v_delayed = v(idx, max(1, cnt-1-delay_idx));
            %velocity attack function
            alpha_v = getAttackVel(cnt,t,vel_flag,attack_vector,...
                r,vf,v_delayed,vel_att);
            v_delayed = v_delayed+alpha_v ; 
            v_delayed = max(v_delayed,0.0);
            v_delayed = min(v_delayed,vf);
            x_delayed = mod(x(idx, max(1, cnt-1-delay_idx)),L); % tcomm_delay was stored earlier
            x_approx = (v_delayed*(delay_idx*t_delt));
            
            %% Count number of cars in front - to attack this, this means ...
            %% through hacking single sensor communication, electromagnetic disturbance, or DOS it physically
            %fprintf('Delay idx is %d, cnt is %d, and tcomm_delay is %d\n', delay_idx, cnt, tcomm_delay(idx));
            veh_ring_pos = mod(x(:,cnt-1), L);  % controller can only detect number of cars at current time
            cars_in_front = 0; % calculate how many cars are before this car near the intersection
            for b = 1:M 
                if(veh_ring_pos(b) > veh_ring_pos(idx) && veh_ring_pos(b) <= 890)
                    cars_in_front = cars_in_front + 1;
                end
            end
            
            %% determine attacked value of number of cars (if there is)
            alpha_cif = getAttackCars(cnt,t,cif_flag,t_delt,n_delt,rho,...
                dsrc_range,attack_vector,r,...
                cars_in_front,cif_att);
            cars_in_front = max([cars_in_front + alpha_cif, 0]);
            cars_in_front = min([cars_in_front, maxCars]);
            
            %% WITH GPS ERROR AND TCOMM AND ADJUSTMENT
            x_err_alpha = 0.0;
            x_err = -5 + (5+5)*rand(1); 
            x_err_alpha = getAttackGPSError(cnt,t,gps_flag,...
                attack_vector,  r, gps_att);

            % if x_err_alpha is nonzero
            currpos = (x_delayed+x_err+x_approx+x_err_alpha);
            if(currpos >= 890.0)
                currpos = 889.9;
            elseif currpos < comm_range
                currpos = comm_range;
            end
            ti1 = currtime+(890.0-currpos)/vf;
            ti2 = currtime+((cars_in_front) * 2); %2=3600/1800 = service rate = seconds/car
            ti_arr(idx) = max([ti1,ti2]);
            if(mod(ti_arr(idx),T_time) >= g_time+y_time) %assign this car to the next green time
                ti_arr(idx) = ti_arr(idx)+T_time-mod(ti_arr(idx),T_time);
            end
            v_asl = (890.0-currpos)/(ti_arr(idx)-currtime);
%             %% Determine attacked asl velocity value
%             [v_asl] = getAttackVelASL(cnt,t,v_asl_flag,attack_vector,...
%                 r,vf,v_asl,vel_asl_att);
            %%%%%v_asl = alpha_v_asl;
            v_asl = max([0.0,v_asl]); % make sure it is at least 0.
            max_vel = min([v_asl, vf]); % make sure it doesn't go past vf, the actual speed limit
            asl_given(idx) = 1; % set this flag so we dont give it a new asl until it's next lap
            vel_asl(idx) = (max_vel);
            tcomm_update(idx) = 0;
            tcomm_delay(idx) = 0;
            %% LOGGING - If desired
%             fprintf('Vehicle %d has true position of %f2 \n', idx, x_delayed);
%             fprintf('Vehicle %d has estimated position of %f2 \n', idx, currpos);
%             fprintf('Vehicle %d has arrival time of %f2 \n', idx, ti_arr(idx));
%             fprintf('Vehicle %d has new velocity of %f2 \n', idx, max_vel);
           
%         fprintf('Expected arrival time is %f aka %f and distance that will be covered is %f\n', ...
%             ti_arr(idx), ti_arr(idx)/t_delt, max_vel * (ti_arr(idx) - currtime));
%         fprintf('Actual maximum velocity used by this car %d is %f\n\n', idx, max_vel);
        else %cnt still less than tcomm_update(idx); but the car has past the loop detector
            max_vel = vf;
            vel_asl(idx) = vf;
        end
        %% Car is in communication range, past the sensor, and they have received V_asl
    elseif isConnected(idx) == 1 && ...
            mod_curr_pos < 890 && ...
            mod_curr_pos >= comm_range && ...
            asl_given(idx) == 1
        %% When actual time for update has not yet happened
        if(tcomm_update(idx) == 0)
            r = rand(); % used for random chance to perform attacks on this car
            tcomm_sample=random(t_pd);
            tcomm_delay(idx)= max([t_delt,tcomm_sample])/t_delt; % this is delay it takes for packet from car to rsu
            tcomm_update(idx)=cnt-1+tcomm_delay(idx); 
            alpha_tcomm = ...
                getAttComm(cnt,t_delt,tcomm_flag,...
                    tcomm_mean_dsrc,attack_vector,r,vf,...
                    g_time,y_time,L,L_int,comm_att);
            tcomm_delay(idx)=tcomm_delay(idx) + round((alpha_tcomm/t_delt)); 
            tcomm_delay(idx)=tcomm_delay(idx) + max([t_delt,tcomm_sample])/t_delt; 
            tcomm_update(idx)=cnt-1+tcomm_delay(idx); 
        end
        %% the RSU will now send the packet over to the vehicle
        if (cnt-1==tcomm_update(idx))
            %% Continuous Vasl update when car is in comm range
            %System does not know if it is attacked or not
            tcomm_sample=random(t_pd);
            delay_idx = max([t_delt,tcomm_sample])/t_delt;
            v_delayed = v(idx, max(1, cnt-1-delay_idx));
            %velocity attack function
            alpha_v = getAttackVel(cnt,t,vel_flag,...
                attack_vector,r,vf,v_delayed,vel_att);
            v_delayed = v_delayed+alpha_v ; 
            v_delayed = max(v_delayed,0.0);
            v_delayed = min(v_delayed,vf);
            x_delayed = mod(x(idx, max(1, cnt-1-delay_idx)),L); % tcomm_delay was stored earlier
            x_approx = (v_delayed*delay_idx*t_delt);
            
            %gps/position attack/normal error function
            x_err_alpha = 0.0;
            x_err = -5 + (5+5)*rand(1); 
            x_err_alpha = getAttackGPSError(cnt,t,gps_flag,...
                attack_vector,  r, gps_att);
            cars_in_front = 0;
            
            %% Worst case attack: TODO 
% %             if(worstcase_flag > 0)
% %                 [alpha_v_asl, x_err_alpha] = getWorstCaseAttackerASL(...
% %                     cnt,t_sec,attack_flags,attack_vector,x_delayed,...
% %                     cars_in_front,ti_arr(idx),vf,vel_asl(idx));
% %             else
% %                 x_err_alpha = 0;
% %             end
            % we change the x_err to negative if time is in green time, else leave alone 
%             if(mod(ti_arr(idx),T_time) < g_time+y_time)
%                 x_err = -x_err;
%             end

            % estimated position of car at current time (but not necessarily actual)
            currpos = (x_delayed+x_err+x_err_alpha+x_approx); 
            if(currpos >= 890.0)
                currpos = 889.9;
            elseif currpos < comm_range
                currpos = comm_range;
            end
            currtime = t_sec;
            
            if(ti_arr(idx) <= currtime)
               ti_arr(idx) = currtime + (890.0-currpos)/vf; 
            end
            if(mod(ti_arr(idx),T_time) >= g_time+y_time)
                ti_arr(idx) = ti_arr(idx)+T_time-mod(ti_arr(idx),T_time);
            end
            max_vel = min([vf,...
                (890.0 - currpos)/(ti_arr(idx)-currtime)]);            

            max_vel = max([0.0,max_vel]);
            %% LOGGING - If desired
%             fprintf('Vehicle %d has true position of %f2 \n', idx, x_delayed);
%             fprintf('Vehicle %d has estimated position of %f2 \n', idx, currpos);
%             fprintf('Vehicle %d has arrival time of %f2 \n', idx, ti_arr(idx));
%             fprintf('Vehicle %d has new velocity of %f2 \n', idx, max_vel);
            vel_asl(idx) = max_vel;
            tcomm_update(idx) = 0;
            tcomm_delay(idx) = 0;
        else % since this is not yet the time for it to have the new asl, we need to provide it the last asl that it was given
            max_vel = vel_asl(idx);
            vel_asl(idx) = max_vel;
        end
    else
        %% When the car is not in communication range
        %         fprintf('It is time %d and this car is %d\n', cnt-1, idx);
        max_vel = vf;
        vel_asl(idx) = vf;
        asl_given(idx) = 0;
        tcomm_update(idx) = 0;
        tcomm_delay(idx) = 0;
    end
    
    %% Vehicle Position and Velocity Update Step - Car-Following Equations
    %% May be altered based on desired car-following model to use
    % update the vehicle speed and position using ASL velocity as max
    % velocity
    [new_vel,new_pos] = ...
        car_following_update(idx,prev,x,v,L,L_int,cnt,t,GT,Y,T,n_delt,t_delt, ...
        rho,tau,max_vel,leading_car,flag_ba,ba);
    % update the velocities and positions
    last_vel = v(idx,cnt-1);
    last_pos = x(idx,cnt-1);
    v(idx,cnt)=new_vel; 
    x(idx,cnt)=new_pos; 
    
    %%%%%%%%%%ENTIRE PATH TRACES FOR SUMO AND OTHER TRAFFIC SIMULATION TOOLS%%%%%%%%%%%
    % for each position update, we can check if car has exited loop and re-entered it, and
    % then that is the time we will insert the vehicle into the
    % intersection in SUMO
    
    % format is: 
    % <vehicle id="0" type="type1" route="route0" depart="0" color="1,0,0"/>
    % arrivalPosLat (aka ring road position of this car)
    % velocity = (aka ring road velocity of this car) 
% % if(connectivity==1)
% % fileIDEW = fopen('ASLRingRoadTraceEW.txt','at');
% % fileIDNS = fopen('ASLRingRoadTraceNS.txt','at');
% % else
% % fileIDEW = fopen('RingRoadTraceEW.txt','at');
% % fileIDNS = fopen('RingRoadTraceNS.txt','at');
% % end
% %     %if(mod(new_pos,L) <= 50.0 && mod(last_pos,L) >= 850.0)
% %     if(t==0)
% %       %  fprintf('Updating index of car %d to be %d\n', idx, idx+(i*M)-1);
% %       %  fprintf('The time is %d and the current car pos and vel are: %f, %f\n', cnt, mod(new_pos,L), mod(new_vel,L));
% %       %  fprintf('The old position and velocity were: %f, %f\n', mod(last_pos,L), mod(last_vel,L));
% %        % last_car_idx(idx) = idx + (i * M) - 1;
% %         fprintf(fileIDEW,...
% %             '<vehicle id="%d" type="ASL_VTYPE" route="eastwest" depart="%d" departSpeed="%f" departPos="%f" color="1,1,0"/>\n',...
% %             idx + (round(cnt/T) * M) - 1, round(cnt*1.5), mod(last_vel,L), mod(new_pos,L));
% %         fprintf(fileIDNS,...
% %             '<vehicle id="%d" type="ASL_VTYPE" route="northsouth" depart="%d" departSpeed="%f" departPos="%f" color="1,1,0"/>\n',...
% %              idx + (round(cnt/T) * M) - 1, round(cnt*1.5), mod(last_vel,L), mod(new_pos,L));
% %     end
% %     fclose(fileIDEW);
% %     fclose(fileIDNS);
% %     %%%%%%%%%%END FOR SUMO%%%%%%%
  i=i+1;
  idx = mod(idx,M)+1;
end
end

%% TCOMM DELAY ATTACK
%% Function to return attack value on communication delay - v2i attack
function[alpha_comm] = getAttComm(cnt,t_delt,tcomm_flag,...
    tcomm_mean,attack_vector,r,vf,...
    g_time,y_time,L,L_int,comm_att)
%% ATTACK MODEL: MODIFIED COMMUNICATION DELAY 
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

% DELAY MUST BE MULTIPLE OF T_DELT FOR THIS TO WORK
alpha_comm = 0.0;
T=60/t_delt;
stea_min_comm_att = max(tcomm_mean, t_delt);
stea_max_comm_att = max(2*tcomm_mean,2*t_delt);
med_max_comm_att = max(5*tcomm_mean,5*t_delt);
extr_max_comm_att = max(10*tcomm_mean,10*t_delt);
max_comm_att = att_duration; % entire duration

if cnt >= att_start && cnt <= (att_start + att_duration)...%在attack_newell_single里面有定义
        && r <= time_att_ratio && isPeriodic == 0% within attack time window
    r_veh = rand(); % probability of attacking each car
    if(r_veh > veh_att_ratio)
        return;
    end
    switch(tcomm_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme
            alpha_comm = med_max_comm_att + ...
                ((rand()*(extr_max_comm_att - med_max_comm_att)));
        case 2 % random: stealthy: 
            alpha_comm = stea_min_comm_att + ...
                ((rand()*(stea_max_comm_att - stea_min_comm_att)));
        case 3 % random: medium - 2x-5x
            alpha_comm = stea_max_comm_att +  ...
                ((rand()*(med_max_comm_att - stea_max_comm_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_comm = comm_att;
        case 5 % for DOS on comm error
            alpha_comm = max_comm_att;
        case 6 % delay
    end
    %%periodic attack with offset between each repeated attack session
    %t % (T_alpha + attack_offset) <= T_alpha seconds; then attack; else do not 
elseif  isPeriodic > 0 && cnt >= att_start && ...
                mod(cnt, att_period + att_offset) <= att_period  && ... 
                cnt <= att_start + att_duration
        r_veh = rand(); % probability of attacking each car
        if(r_veh > veh_att_ratio)
            return;
        end
        switch(tcomm_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme
            alpha_comm = med_max_comm_att + ...
                ((rand()*(max_comm_att - med_max_comm_att)));
        case 2 % random: stealthy:
            alpha_comm = stea_min_comm_att + ...
                ((rand()*(stea_max_comm_att - stea_min_comm_att)));
        case 3 % random: medium -
            alpha_comm = stea_max_comm_att +  ...
                ((rand()*(med_max_comm_att - stea_max_comm_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_comm = comm_att;
        case 5 % for DOS on comm error
            alpha_comm = max_comm_att;
        case 6 % delay

        case 7 % constrained range due to budget

        end  
%else
end    
end

%% GPS XERR ATTACK
%% Function to return attack value of GPS position of car - GPS sensor attack
function [alpha_gps] = getAttackGPSError(cnt,t,gps_flag,...
    attack_vector,r,gps_att)
%% ATTACK MODEL: TAMPERED GPS SIGNAL %pos error
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

% we'll assume the attack can cause this to be a value from [-10, 10]
alpha_gps = 0; % static value (negative if the time is in green time, positive if the time is in red time)
L_dsrc = 300;
stea_min_gps_att = 1;
stea_max_gps_att = L_dsrc/10;
med_max_gps_att = L_dsrc/2;
max_gps_att = L_dsrc-1;

if cnt >= att_start && cnt <= (att_start + att_duration)...%在attack_newell_single里面有定义
        && r <= time_att_ratio && isPeriodic == 0% within attack time window
    r_veh = rand(); % probability of attacking each car
    if(r_veh > veh_att_ratio)
        return;
    end
    switch(gps_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme
            alpha_gps = med_max_gps_att + ...
                ((rand()*(max_gps_att - med_max_gps_att)));
        case 2 % random: stealthy: 1-10 cars
            alpha_gps = stea_min_gps_att + ...
                ((rand()*(stea_max_gps_att - stea_min_gps_att)));
        case 3 % random: medium - reserved and near average 10-21
            alpha_gps = stea_max_gps_att +  ...
                ((rand()*(med_max_gps_att - stea_max_gps_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_gps = gps_att;
        case 5 % for DOS on gps error
            alpha_gps = max_gps_att;
        case 6 % delay
    end
    %%periodic attack with offset between each repeated attack session
    %t % (T_alpha + attack_offset) <= T_alpha seconds; then attack; else do not 
    elseif  isPeriodic > 0 && cnt >= att_start && ...
                mod(cnt, att_period + att_offset) <= att_period  && ... 
                cnt <= att_start + att_duration
        r_veh = rand(); % probability of attacking each car
        if(r_veh > veh_att_ratio)
            return;
        end
        switch(gps_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme
            alpha_gps = med_max_gps_att + ...
                ((rand()*(max_gps_att - med_max_gps_att)));
        case 2 % random: stealthy: 1-10 cars
            alpha_gps = stea_min_gps_att + ...
                ((rand()*(stea_max_gps_att - stea_min_gps_att)));
        case 3 % random: medium - reserved and near average 10-21
            alpha_gps = stea_max_gps_att +  ...
                ((rand()*(med_max_gps_att - stea_max_gps_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_gps = gps_att;
        case 5 % for DOS on gps error
            alpha_gps = max_gps_att;
        case 6 % delay

        case 7 % constrained range due to budget

        end  
%else
end    
end

%% INDUCTION LOOP ATTACK
%% Function to return attack value of number of cars - induction loop attack
function [alpha_cif] = getAttackCars(cnt,t,cif_flag,t_delt,n_delt,rho,...
    dsrc_range,attack_vector,r,cif_old, cif_att)

%% ATTACK: CARS_IN_FRONT (CIF): TI_N (ARRIVAL TIME TO INTERSECTION)
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

% maximum number of cars: floor(dsrc_range/(n_delt*rho))
alpha_cif = 0;
% 41 since this car is included
max_cif_att = floor(dsrc_range/(n_delt*rho))-1; 
stea_min_cif_att = 1;
stea_max_cif_att = floor(max_cif_att/4);
med_max_cif_att = floor(max_cif_att/2);
if cnt >= att_start && cnt <= (att_start +att_duration) ...
        && r <= time_att_ratio && isPeriodic == 0 % within attack time window
    r_veh = rand(); % probability of attacking each car
    if(r_veh > veh_att_ratio)
        return;
    end
    switch(cif_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme 21-41
            alpha_cif = med_max_cif_att + ...
                ceil((rand()*(max_cif_att - med_max_cif_att)));
        case 2 % random: stealthy: 1-10 cars
            alpha_cif = stea_min_cif_att + ...
                ceil((rand()*(stea_max_cif_att - stea_min_cif_att)));
        case 3 % random: medium - reserved and near average 10-21
            alpha_cif = stea_max_cif_att +  ...
                ceil((rand()*(med_max_cif_att - stea_max_cif_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_cif = cif_att;
        case 5 % for DOS on gps error
            alpha_cif = max_cif_att;
        case 6 % delay
            
        case 7 % constrained range due to budget
            
    end
%%periodic attack with offset between each repeated attack session
    elseif  isPeriodic > 0 && cnt >= att_start && ...
                mod(cnt, att_period + att_offset) <= att_period  && ... 
                cnt <= att_start + att_duration
    r_veh = rand(); % probability of attacking each car
    if(r_veh > veh_att_ratio)
        return;
    end
    switch(cif_flag) % depending on flag, we modify velocity accordingly
        case 1 % random: extreme
            alpha_cif = med_max_cif_att + ...
                ceil((rand()*(max_cif_att - med_max_cif_att)));
        case 2 % random: stealthy: 1-10 cars
            alpha_cif = stea_min_cif_att + ...
                ceil((rand()*(stea_max_cif_att - stea_min_cif_att)));
        case 3 % random: medium - reserved and near average 10-21
            alpha_cif = stea_max_cif_att +  ...
                ceil((rand()*(med_max_cif_att - stea_max_cif_att)));
        case 4 % deterministic - given value - can be anything set by attacker
            alpha_cif = cif_att;
        case 5 % for DOS gps error
            alpha_cif = max_cif_att;
        case 6 % delay
            
        case 7 % constrained range due to budget
            
    end  
end
end


%flag 4
%% VELOCITY ATTACK
%% Function to return attack value of asl velocity - traffic controller attack
function [alpha_v] = getAttackVel(cnt,t,v_flag,...
    attack_vector,r,vf,v_old,vel_att)
%% ATTACK MODEL: MODIFIED VEHICLE VELOCITY
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

alpha_v = 0;

max_v_att = vf; 
stea_min_v_att = 1;
stea_max_v_att = max_v_att/5;
med_max_v_att = max_v_att/2;

if cnt >= att_start && cnt <= (att_start +att_duration)...
        && r <= time_att_ratio && isPeriodic == 0 % within attack time window
    r_veh = rand(); % probability of attacking each car
    if(r_veh > veh_att_ratio)
        return;
    end
    
    switch(v_flag)
        case 2 % random: stealthy: 1-3 cars
            alpha_v = stea_min_v_att + ...
                ((rand()*(stea_max_v_att - stea_min_v_att))); 
    end

%%periodic attack with offset between each repeated attack session
    elseif  isPeriodic > 0 && cnt >= att_start && ...
                mod(cnt, att_period + att_offset) <= att_period  && ... 
                cnt <= att_start + att_duration
end
end


% flag 5
%% ASL VELOCITY ATTACK 
%% Function to return attack value of asl velocity - traffic controller attack
function [v_asl] = getAttackVelASL(cnt,t,v_asl_flag,...
    attack_vector,r,vf,v_asl_old,vel_asl_att)
%% ATTACK MODEL: MODIFIED VEHICLE VELOCITY
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

% init return value to the old value
v_asl = v_asl_old;

% v_asl: can take on values from [0, vf]; meters/second
% attack values can also take values from [0, vf]
% attack perhaps should depend on the original v_asl value
minatt_vasl = -vf;
maxatt_vasl = vf;
if cnt >= att_start && cnt <= (att_start +att_duration)...
        && r <= time_att_ratio && isPeriodic == 0 % within attack time window
    r_veh = rand();
    if(r_veh > veh_att_ratio)
        return;
    end
%     switch(v_asl_flag) % depending on flag, we modify velocity accordingly
%     end
%%periodic attack with offset between each repeated attack session
elseif  isPeriodic > 0 && cnt >= att_start && ...
            mod(cnt, att_period + att_offset) <= att_period  && ... 
            cnt <= att_start + att_duration
end
end

%% WORST-CASE ATTACK MODEL: TODO
% the worst-case aka intelligent attacker will use the targeted variable(s)
% to make sure that the vehicle always arrives on the red time (unless 
% the vehicle is too close to the intersection)
% It is important that the attacker uses either x, tcomm, or vel_asl
% to perform the attack since at every time step after the first time it receives asl
% the vehicle will receive a new valid one and will reduce the impact of
% the attack.
function [hat_v_asl, x_alpha] = ...
    getWorstCaseAttackerASL(cnt,currtime,attack_flags,...
    attack_vector,x,cars_in_front,ti,vf,v_asl_old)

% attack flags can be used to identify which variables are targeted to
% perform the attack

    %% get the parameters
att_start=attack_vector(1);
att_duration=attack_vector(2); 
time_att_ratio=attack_vector(3);
isPeriodic=attack_vector(4);
att_period=attack_vector(5);
att_offset=attack_vector(6);
veh_att_ratio=attack_vector(7);

    hat_v_asl = 0.0;
    v_delt = 0;
    x_delt = 0;
    tcomm_delt = 0;
    v_delt = 0;
    L1 = 890;
    t_delta = 1.5;
    R = 30.0/(t_delta);
    T = 60.0/(t_delta);
    g_time = 24.0;
    y_time = 6.0;
    T_time = 60.0;
    x_err_est = -5 + (5+5)*rand(1);
    sr = 1800/3600;
    curr_step = mod((cnt-1),T);
    mod_time = mod(currtime,T_time);
    
    % need to predict the new ti if t > ti
    if(currtime > ti)
        ti = currtime + (890-(x+x_err_est))/vf; 
        if(mod(ti,T_time) >= g_time+y_time) % need to verify if this is correct logic
            ti = ti+T_time-mod(ti,T_time);
        end
    end
    
    % 
%     if(ti >= currtime + (T_time-mod(currtime, T_time)))
%         hat_v_asl = vf;
%         x_alpha = 0;
%         return;
%     end
%     % 
%     
    dist_to_inter = L1 - (x+x_err_est);
    
    if(mod(currtime,T_time) >= g_time + y_time) 
       hat_v_asl = vf;
       x_alpha = -(300-dist_to_inter);
       return;
    end
    
    %% Attack 
    % goal is: v_asl * (R-t) = 900-x = dist to inter
    g_t_left = 0;
    if(mod_time <= 29.0)
        g_t_left = (29.0-mod_time);
    else
        g_t_left = (T_time-mod_time)+29.0;
    end
    
%     if(g_t_left * vf < L1 - (x+x_err_est))
%         hat_v_asl = vf;
%         x_alpha = 0;       
%         return;
%     end
        
    hat_v_asl = g_t_left/dist_to_inter;
    hat_v_asl = min(hat_v_asl, vf);
    
%     x_alpha = dist_to_inter-(hat_v_asl * ...
%         (g_t_left));
    x_alpha = dist_to_inter * (1-((ti - currtime)/g_t_left));
    % hat_x_alpha = (L1-x)(1-(ti-t)/(tr-t))
end