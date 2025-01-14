function [A,B] = createAB(m, g, l, I_P, I_F)
    % System Dynamics
    f_1 = -(m * g * l) / (I_P + m * (l^2));
    f_2 = (m * g * l) / (I_P + m * (l^2));
    f_3 = -(1 / (I_P + m * (l^2)));
    f_4 = 1 / I_F + 1 / (I_P + m * (l^2));
    
    A = [0, 1, 0, 0;
         f_1, 0, 0, 0;
         0, 0, 0, 1;
         f_2, 0, 0, 0];
    
    B = [0;
         f_3;
         0;
         f_4];

end

