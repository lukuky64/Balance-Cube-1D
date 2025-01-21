function checkControllableFromState(A, B, x0, u_max, u_rate_max, dt)
    % Compute the Controllability Matrix
    n = size(A, 1); % Number of states
    C = [];
    for i = 0:n-1
        C = [C, A^i * B]; % Build the controllability matrix
    end

    % Check if x0 lies in the column space of C
    alpha = pinv(C) * x0; % Solve C * alpha = x0
    x0_approx = C * alpha; % Reconstruct x0 from the controllability matrix
    is_controllable = norm(x0 - x0_approx) < 1e-6;

    % Simulate control effort and rate constraints
    if is_controllable
        u_prev = 0; % Initial control input
        x = x0; % Start from the initial state
        time_limit = 5; % Time limit to reach the desired state
        num_steps = round(time_limit / dt);
        feasible = true; % Flag for feasibility of control within constraints

        for step = 1:num_steps
            % Compute control input
            u = pinv(B) * (-A * x);

            % Clamp the control input to u_max
            if abs(u) > u_max
                u = sign(u) * u_max;
            end

            % Apply rate limiting
            u_dot = (u - u_prev) / dt; % Rate of change
            if abs(u_dot) > u_rate_max
                u = u_prev + sign(u_dot) * u_rate_max * dt;
            end

            % Update state using Euler integration
            x_dot = A * x + B * u;
            x = x + x_dot * dt;

            % Update the previous control input
            u_prev = u;

            disp(norm(x));

            % Check if the state is sufficiently close to zero
            if norm(x) < 1e-1
                break; % Exit the loop if the state is stabilised
            end

            % If time runs out without stabilising
            if step == num_steps
                feasible = false;
            end
        end

        % Display results
        if feasible
            disp('The system is controllable from the given initial state within control limits.');
        else
            disp('The system is controllable, but constraints make it infeasible to stabilise within the time limit.');
        end
    else
        disp('The system is NOT controllable from the given initial state.');
    end
end