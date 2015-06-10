function plotFFT()

[x,y] = textread('accels.txt','%d %f');

if mod(length(x),2) == 1    %x should have an even number of samples
    x = [x 0];              %if not, pad with zero
end

b= fir1(10,0.2);
fir = conv(b,x);
c= [0.2 0.2 0.2 0.2 0.2];
maf = conv(c,x);

N = length(fir);
X = fft(fir);
mag(1) = abs(X(1))/N;               %DC component
mag(N/2+1) = abs(X(N/2+1))/N;       %Nyquist frequency component
mag(2:N/2) = 2*abs(X(2:N/2))/N;     %all other frequency components
freqs = linspace(0,1,N/2+1);        %make x-axis as fraction of Nyquist freq
subplot(2,1,1);
stem(freqs, mag);                   %plot the FFT magnitude plot
axis([-0.05 1.05 -0.1*max(mag) max(mag)+0.1*max(mag)]);
xlabel('Frequency (as fraction of Nyquist frequency)');
ylabel('Magnitude');
title('Single-sided FFT magnitude');


subplot(2,1,2);
plot(fir);

set(gca,'FontSize',18);
