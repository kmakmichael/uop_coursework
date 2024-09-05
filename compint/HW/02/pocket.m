function [gb,abo,abi] = pocket(xd, yd, g_p, a_p, N)
    % make the pocket
    g = [0;1;0.5];
    a = 0;
    % data prep
    [x_t,y_t,x,y] = split_data(xd,yd,N);
    % train 
    [g,~] = pla(x_t, y_t);
    % test
    a = pla_test(x,y,g);
    % return the better weights
    if a_p > a
        gb = g_p;
        abo = a_p;
        abi = pla_test(x_t,y_t,g_p);
    else
        gb = g;
        abo = a;
        abi = pla_test(x_t,y_t,g);
    end
end