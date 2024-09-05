function u = correlate_ssd(i0, i1, c0, c1)
    Ecc = 0
    for i = 1:height(c0)
        Ecc = Ecc + i1(
    [u1, u2] = find(Ecc==max(Ecc, [], 'all'));
    u = [u1 u2];
end