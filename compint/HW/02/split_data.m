function [x_t,y_t,x,y] = split_data(xd,yd,N)
    x_t = zeros(N,3);
    y_t = zeros(N,1);
    x = zeros(height(xd)-N,3);
    y = zeros(height(xd)-N,1);
    for n = 1:N
        r = randi(height(xd));
        x_t(n,1) = 1;
        x_t(n,2) = xd(r,1);
        x_t(n,3) = xd(r,2);
        y_t(n) = yd(r);
        xd(r,:) = [];
        yd(r) = [];
    end
    x = horzcat(ones(height(xd),1),xd);
    y = yd;
end