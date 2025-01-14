function checkControllableFromState(A, B, x0, u_max)
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

    % Compute required control input and check against u_max
    if is_controllable
        u_required = norm(alpha); % Approximation of required control effort
        if abs(u_required) <= u_max
            disp('The system is controllable from the given initial state within control limits.');
        else
            disp('The system is controllable from the given initial state, but u EXCEEDS u_max.');
        end
    else
        disp('The system is NOT controllable from the given initial state.');
    end
end