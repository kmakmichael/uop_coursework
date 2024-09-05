function u = correlate(i0, i1)
    Ecc = ifft(fft(i0) .* fft(conj(i1)));
    [u1, u2] = find(Ecc==max(Ecc, [], 'all'));
    u = [u1 u2];
end