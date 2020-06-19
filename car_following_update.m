%% Newell car-following update - This is different for normal vs extended.
% if the car is not within the communication range, it will not get an
% ASL velocity but instead use the normal vf.
% check if the car in front of this one has a position on next lap
function [new_vel,new_pos] = ...
    car_following_update(idx,prev,veh_pos,veh_vel,L,L_int,cnt,t,GT,Y,T,n_delt,t_delt, ...
    rho,tau,max_velocity,leading_car,flag_ba,ba)
DEBUG = 0;
veh_acc = ba;
% curr -> current/following car (id is idx)
% prev -> leading car (in front of current car) (id is prev)
curr_pos = veh_pos(idx,cnt-1);
prev_pos = veh_pos(prev,cnt-1);
curr_vel = veh_vel(idx,cnt-1);
prev_vel = veh_vel(prev,cnt-1);
mod_curr_pos = mod(curr_pos,L);
mod_prev_pos = mod(prev_pos,L);
separation = mod_prev_pos - mod_curr_pos;
if(separation < 0)
    %         fprintf('Current position %f and prev car position is %f\n', mod_curr_pos, mod_prev_pos);
    separation = mod(mod_prev_pos - mod_curr_pos,L);
end
if(separation >= 900)
    fprintf('Something went wrong. Distance between two cars is greater/equal than 900');
end

%% UNBOUNDED ACCELERATION
if(flag_ba ~= 1)
    % check if this car is now the new leading car or not;then update it
    % particularly we only care about the leading car when the lights
    % hit red, so will determine how much it will slow down according
    % to imaginary car in intersection. if car is connected,, they
    % will still use ASL velocity provided. otherwise, vf is the max
    if (idx == leading_car && t >= GT + Y) 
        % we create virtual vehicle to stop this car
        virt_veh_pos = max([(ceil((curr_pos/L)) * L) - L_int,L-L_int]);
        %% SIMPLIFIED NEWELL - maximum covered distance = vf * t_delt
        new_vel = min([max_velocity ]);
        % we assume car can come to complete stop in a flash for now
        new_pos = min([curr_pos+(t_delt*max_velocity),...
            virt_veh_pos]);
        if new_pos==virt_veh_pos
            new_vel = 0;
        end
    else
        new_vel = min([max_velocity,...
            (separation-rho*n_delt)/(tau*n_delt)]);
        new_vel = max([new_vel, 0]);
        new_pos = min([curr_pos+(t_delt*max_velocity),...
            curr_pos + t_delt*(separation-rho*n_delt)/(tau*n_delt)]);
        new_vel = max([new_vel,0]);
        new_pos = max([new_pos, curr_pos]);
        %% LOGGING
        %         fprintf('Time is %d\n', cnt-1);
        %         fprintf('Maximum velocity for car %d is %f\n', idx, max_velocity);
        %         fprintf('Separation is: %f\n', separation);
        %         fprintf('Speed based on separation is: %f\n',(separation-rho*n_delt)/(tau*n_delt));
        %         fprintf('new_vel is %f\n', new_vel);
        %         fprintf('Max position allowed for this car is :%f\n', mod(mod_prev_pos - rho*n_delt,L));
        %         fprintf('Old position of this car is %f and new position is %f\n',...
        %         curr_pos, new_pos);
        % make sure that all cars are at least jam spacing between
        % each other when light is red and leading car stops abruptly
        % at intersection line
        %         if(mod(new_pos,L) >= 890 && mod(curr_pos,L) < 890)
        %            fprintf('Car is %d, it arrived at intersection at time %f \n', idx, (cnt-1)*t_delt);
        %            fprintf('-------\n')
        %         end
        
        %% If there is a jam on other side of intersection...
        D_veh_int = max(0,(890.0-mod_curr_pos)); 
        if (idx==leading_car && D_veh_int <= rho*n_delt && ...
                mod_prev_pos <= rho*n_delt && prev_vel == 0.0) 
           if(mod_curr_pos > 890.0)
               curr_pos = curr_pos + D_veh_int;
               mod_curr_pos = mod_curr_pos + D_veh_int;
           end
           virt_veh_pos = max([(ceil((curr_pos/L)) * L) - L_int,L-L_int]);
           separation =  min([D_veh_int+7.0,separation]);
           new_pos = min([curr_pos+(t_delt*max_velocity), ...
                curr_pos + t_delt*(separation-rho*n_delt)/(tau*n_delt),...
                virt_veh_pos]);
           new_vel = min([max_velocity,...
                (separation-rho*n_delt)/(tau*n_delt) ]);
           new_vel = max([0,new_vel]);
        end
    end

%% BOUNDED ACCELERATION
% Need to consider the acceleration of the car and whether or not it can
% stop in time. We will assume all are aggressive for simplicity.
else
% check if this is leading car and can stop in time with current position and speed 
%if(idx == leading_car && t >= GT + Y)
D_veh_int = (890.0-mod_curr_pos); 
D_run = (curr_vel*(GT+Y-t)); %curr_vel * (16 + 4 - t)
% this car is leader and cannot make it thru the intersection successfully
% in time - either because it is too far away during yellow time or because
% it is red time or because there is vehicle on other end?
% this car is leading but already passed line
    if ((idx == leading_car && t >= GT && t < GT+Y && ...
            D_veh_int < D_run))
        new_vel = min([max_velocity,...
            (separation-rho*n_delt)/(tau*n_delt)]);
        new_vel = min([new_vel, curr_vel + (t_delt * veh_acc)]); %bounded
        new_vel = max([new_vel, 0]);
        new_pos = min([curr_pos+(t_delt*max_velocity),...
            curr_pos + t_delt*(separation-rho*n_delt)/(tau*n_delt)]);
        new_pos = min([new_pos, curr_pos + (t_delt*curr_vel) + (t_delt^2 * veh_acc)]); % bounded
        new_vel = max([new_vel,0]);
        new_pos = max([new_pos, curr_pos]);
    elseif (idx == leading_car && t >= GT)  
        % we create virtual vehicle to stop this car
        virt_veh_pos = max([(ceil((curr_pos/L)) * L) - L_int,L-L_int]);
        separation = min([D_veh_int+7.0,separation]);
        %separation = D_veh_int + 7.0;
        new_vel = min([max_velocity,...
            (separation-rho*n_delt)/(tau*n_delt) ]);
        % this incorporates deceleration of 3.0 m/s^2
        %new_vel = min([new_vel, curr_vel + (t_delt * -veh_acc)]);
        % we assume car can come to complete stop in a flash for now
        new_pos = min([curr_pos+(t_delt*max_velocity), ...
            curr_pos + t_delt*(separation-rho*n_delt)/(tau*n_delt),...
            virt_veh_pos]);
        % this incorporates deceleration of 3.0 m/s^2
        %new_pos = min([new_pos, curr_pos + (t_delt*curr_vel) + (t_delt^2 * -veh_acc)]);
        if new_pos==virt_veh_pos
            new_vel = 0;
        end
        new_vel = max([new_vel,0]);
        new_pos = max([new_pos, curr_pos]);
    else % not leading car
        new_vel = min([max_velocity,...
            (separation-rho*n_delt)/(tau*n_delt)]);
        new_vel = min([new_vel, curr_vel + (t_delt * veh_acc)]); %bounded
        new_vel = max([new_vel, 0]);
        new_pos = min([curr_pos+(t_delt*max_velocity),...
            curr_pos + t_delt*(separation-rho*n_delt)/(tau*n_delt)]);
        new_pos = min([new_pos, curr_pos + (t_delt*curr_vel) + (t_delt^2 * veh_acc)]); % bounded
        new_vel = max([new_vel,0]);
        new_pos = max([new_pos, curr_pos]);
    end
    
    %there is a jam on other side of intersection
     D_veh_int = (890.0-mod_curr_pos); 
     if (idx==leading_car && D_veh_int >= 0 && D_veh_int <= rho*n_delt && ...
             prev_vel <= 1.0 && mod_prev_pos <=rho*n_delt) 
        virt_veh_pos = max([(ceil((curr_pos/L)) * L) - L_int,...
            L-L_int]);
        separation =  min([D_veh_int,separation]);
        new_pos = min([curr_pos+(t_delt*max_velocity), ...
            curr_pos + t_delt*(separation+7.0-rho*n_delt)/(tau*n_delt),...
            virt_veh_pos]);
       new_vel = min([max_velocity,...
            (separation+7.0-rho*n_delt)/(tau*n_delt) ]);
       new_vel = min([new_vel, curr_vel + (t_delt * veh_acc)]); %bounded
       new_vel = max([0,new_vel]);
     end
    
end
%% LOGGING
% fprintf('New position for this vehicle %d is %f\n\n',idx, new_pos);
% fprintf('New velocity for this vehicle %d is %f\n\n',idx, new_vel);
end