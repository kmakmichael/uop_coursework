% A -> B
function img_out = transform(corrs, imgs, A, B)
    T = fitgeotrans(corrs{A,B}, corrs{B,A}, 'projective');
    img_out = imwarp(imgs{A}, T);
end