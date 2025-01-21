classdef ControlSimulator
    properties
        A          % System matrix
        B          % Input matrix
        K          % LQR gain matrix
        u_max      % Maximum control input
        u_rate_max % Maximum control rate of change
        dt         % Time step
    end
    
    methods
        function obj = ControlSimulator(A, B, K, u_max, u_rate_max, dt)
            % Constructor
            obj.A = A;
            obj.B = B;
            obj.K = K;
            obj.u_max = u_max;
            obj.u_rate_max = u_rate_max;
            obj.dt = dt;
        end
        
        function [is_controllable,is_feasible] = checkControllability(obj, x0, display)
            % Check controllability from the given initial state
            n = size(obj.A, 1); % Number of states
            C = [];
            for i = 0:n-1
                C = [C, obj.A^i * obj.B]; % Build the controllability matrix
            end
            
            % Check if x0 lies in the column space of C
            alpha = pinv(C) * x0; % Solve C * alpha = x0
            x0_approx = C * alpha; % Reconstruct x0 from the controllability matrix
            is_controllable = norm(x0 - x0_approx) < 1e-6;
            
            % Simulate feasibility
            is_feasible = obj.simulate(x0, false); % Simulate without plots

            if (display)
                if is_controllable
                    disp('System is theoretically controllable from x0');
                    if is_feasible
                        disp('The system is practically controllable within constraints.');
                    else
                        disp('Practical constraints make the system NOT feasible.');
                    end
                else
                    disp('The system is NOT controllable from the given initial state.');
                end
            end
        end
        
        function feasible = simulate(obj, x0, plot_results)
            % Simulate the system and return feasibility
            if nargin < 3
                plot_results = true; % Default: plot results
            end
            
            % Simulation parameters
            T = 3; % Simulation duration
            time = 0:obj.dt:T;
            
            % Initialise state and control input storage
            x = x0;
            x_vals = zeros(length(time), length(x0)); % Store states over time
            u_vals = zeros(1, length(time)); % Store control inputs over time
            u_prev = 0; % Previous control input (initialised to 0)
            
            % Feasibility check
            feasible = false;
            
            % Simulate system response
            for i = 1:length(time)
                % Store the current state
                x_vals(i, :) = x';
                
                % Compute control input
                u = -obj.K * x;
                
                % Gradual clamping of control input
                if abs(u) > obj.u_max
                    u = sign(u) * obj.u_max;
                end
                
                % Apply rate limiting
                u_dot = (u - u_prev) / obj.dt; % Compute the rate of change
                if abs(u_dot) > obj.u_rate_max
                    u = u_prev + sign(u_dot) * obj.u_rate_max * obj.dt;
                end
                
                u_vals(i) = u; % Store control input
                
                % Update state using Euler integration
                x_dot = obj.A * x + obj.B * u;
                x = x + x_dot * obj.dt;
                
                % Check if the first state (theta) is sufficiently close to zero
                if abs(x(1)) < 1e-3
                    feasible = true;
                end
                
                % Update the previous control input
                u_prev = u;
            end
            
            % Plot results if required
            if plot_results
                obj.plotResults(time, x_vals, u_vals);
            end
        end

        function x0 = findThetaLimit(obj, x0)
            if(sign(x0(1)) == 0)
                disp("Initial theta cannot be zero!");
                return;
            end

            theta_step = -(1e-2); % We will increment towards the counter-clockwise angle
            theta_val = theta_step;
            prev_theta_val = theta_val; 
                        
            while(1)
                x0(1) = theta_val;
                [~, feasible] = obj.checkControllability(x0, false);
                if (~feasible) % Keep incrementing until we aren't controllable
                    x0(1) = prev_theta_val;
                    theta_degrees = x0(1) * 360/(2*pi);
                    fprintf('Controllable limit for theta: %.1f degrees\n', theta_degrees);
                    break;
                end
                prev_theta_val = theta_val;
                theta_val = theta_val + theta_step;
            end
            % Go through iterations of checking feasibility for theta
            % values. We will converge on a value so slowly decrease step
            % size as we find the global max. Only worry about one
            % direction, which should oppsose omega.
        end
        
        function plotResults(~, time, x_vals, u_vals)
            % Plot system states
            figure;
            subplot(2, 2, 1);
            plot(time, x_vals(:, 1), 'LineWidth', 1.5);
            title('Angle (θ)');
            xlabel('Time (s)');
            ylabel('θ (rad)');
            grid on;

            subplot(2, 2, 2);
            plot(time, x_vals(:, 2), 'LineWidth', 1.5);
            title('Angular Velocity (θ dot)');
            xlabel('Time (s)');
            ylabel('θ dot (rad/s)');
            grid on;

            subplot(2, 2, 3);
            plot(time, x_vals(:, 3), 'LineWidth', 1.5);
            title('Reaction Wheel Angle (φ)');
            xlabel('Time (s)');
            ylabel('φ (rad)');
            grid on;

            subplot(2, 2, 4);
            plot(time, x_vals(:, 4), 'LineWidth', 1.5);
            title('Reaction Wheel Angular Velocity (φ dot)');
            xlabel('Time (s)');
            ylabel('φ dot (rad/s)');
            grid on;

            % Add Overall Title for State Plots
            sgtitle('System States with LQR Control');

            % Plot control input
            figure;
            plot(time, u_vals, 'LineWidth', 1.5);
            xlabel('Time (s)');
            ylabel('Control Input (u)');
            title('Control Input vs. Time');
            grid on;
        end
    end
end