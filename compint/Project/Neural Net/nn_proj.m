%% Introduction
% The USPS Handwritten Digit database is a set of handwriting data
% containing 1100 examples of the digits 0-9. To process and learn this
% data, ...
% your two paragraphs go here.
clearvars;

%% Data Preparation
% To prepare for training, data is pulled from the USPS handwriting set and
% processed. |getfeatures()| is called to find the symmetry and intensity
% of each data point.
load('usps_modified.mat');
[xd,yd] = getfeatures(data);
x = nl_transform(xd);
for i = 501:height(yd)
    yd(i) = -1;
end

%% Forward Propogation
% 


%% Backwards Propogation
%

%% Training
%

%% Testing
% 

