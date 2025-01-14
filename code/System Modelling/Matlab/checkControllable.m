function [status] = checkControllable(A,B)
    % Controllability Check
    if rank(ctrb(A, B)) == size(A, 1)
        disp('System is controllable');
        status = true;
    else
        disp('System is not controllable');
        status = false;
    end
end

