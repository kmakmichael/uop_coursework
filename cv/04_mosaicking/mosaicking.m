%% Mosaicking
% more description here

%% load correspondences
[corrs, pairs] = correspondences();
images = cell(12,1);
for im = 1:9
    images{im} = imread(strcat('tillman/small_tillman0', num2str(im), '.jpg'));
end
for im = 10:12
    images{im} = imread(strcat('tillman/small_tillman', num2str(im), '.jpg'));
end

%% correlation
clt4_5 = correlate(images{6}, images{5});
%% homography and warping
for i = 1:height(pairs)
    T = projective2d(homography(corrs{pairs(i,2),pairs(i,1)}, corrs{pairs(i,1),pairs(i,2)})');
    images{pairs(i,2)} = imwarp(images{pairs(i,2)}, T);
end

for i = 1:12
    imwrite(images{i}, strcat('warped/tillman_', num2str(i), '.jpg'));
end

diffs = corrs{5,4} - corrs{4,5};
md = round([mean(diffs(:,1)) mean(diffs(:,2))]);

%% warp the images
% hey, there's nothing here
%% fake
%{
% 6 -> 5
T = fitgeotrans(corrs{6,5}, corrs{5,6}, 'projective');
images{6} = imwarp(images{6}, T);

% 7 -> 9
T = fitgeotrans(corrs{7,9}, corrs{9,7}, 'projective');
images{7} = imwarp(images{7}, T);

% 5 -> 3
T = fitgeotrans(corrs{5,3}, corrs{3,5}, 'projective');
images{5} = imwarp(images{5}, T);

% 5 -> 4
T = fitgeotrans(corrs{5,4}, corrs{4,5}, 'projective');
images{5} = imwarp(images{5}, T);

% 9 -> 10
T = fitgeotrans(corrs{9,10}, corrs{10,9}, 'projective');
images{10} = imwarp(images{10}, T);

% 1 -> 12
T = fitgeotrans(corrs{1,12}, corrs{12,1}, 'projective');
images{1} = imwarp(images{1}, T);

% 2 -> 7
T = fitgeotrans(corrs{2,7}, corrs{7,2}, 'projective');
images{2} = imwarp(images{2}, T);

% 1 -> 11
T = fitgeotrans(corrs{1,11}, corrs{11,1}, 'projective');
images{1} = imwarp(images{1}, T);

% 7 -> 8
T = fitgeotrans(corrs{7,8}, corrs{8,7}, 'projective');
images{7} = imwarp(images{7}, T);

% 7 -> 5
T = fitgeotrans(corrs{7,5}, corrs{5,7}, 'projective');
images{7} = imwarp(images{7}, T);
%}

