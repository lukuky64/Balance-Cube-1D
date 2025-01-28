function [] = simulateSystem(A, B, K, x0, u_max, u_rate_max, dt)
    % Simulation parameters
    T = 3;    % Simulation duration
    time = 0:dt:T;

    % Initialise state and control input storage
    x = x0;
    x_vals = zeros(length(time), length(x0)); % Store states over time
    u_vals = zeros(1, length(time)); % Store control inputs over time
    u_prev = 0; % Previous control input (initialised to 0)

    % Simulate system response
    for i = 1:length(time)
        % Store the current state
        x_vals(i, :) = x';

        % Compute control input
        u = -K * x;

        % Gradual clamping of control input
        if abs(u) > u_max
            u = sign(u) * (u_max - exp(-abs(u) + u_max)); % Gradual damping
        end

        % Apply rate limiting
        u_dot = (u - u_prev) / dt; % Compute the rate of change
        if abs(u_dot) > u_rate_max
            u = u_prev + sign(u_dot) * u_rate_max * dt; % Limit the rate of change
        end

        u_vals(i) = u; % Store control input

        % Update state using Euler integration
        x_dot = A * x + B * u;
        x = x + x_dot * dt;

        % Update the previous control input
        u_prev = u;
    end

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