% pla_getdata: creates a d dimensional data set, x, of size (n,dim) 
% of linearly separable data with labels, y. 
% INPUTS:
%   data range interval [lb, ub] (lower bound, upper bound)
%   d: dimension of data
%   n: total number of data points
%**************************************************************************
function [xn, y, w] = pla_getdata(lb, ub, dim, n)
    x = lb + (ub-lb).*rand(n,dim);  %generate data

    % generate 'unknown' target line to label data
    b = lb + (ub-lb).*rand(1);      %line offset
    temp_w = -1 + 2.*rand(dim,1);   %generate weight vectors
    w0 = -b*temp_w(dim);            %calculate offset weight
    w = [w0; temp_w];               %target weights

    xn = horzcat(ones(n,1),x);      %data set with x_0

    y = zeros(n,1);                 %initialize training output

    %calculate label output using 'unknown' target function
    for i = 1:n
        y(i) = sign(dot(w,xn(i,:)));
    end
end