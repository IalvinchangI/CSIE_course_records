
% MATLAB PROGRAM ecg_hfn.m
clear all               % clears all active variables
close all

ecg = load('ecg_hfn.dat');
fs = 1000; %sampling rate = 1000 Hz

% Original Signal
slen = length(ecg);
t = (1:slen)/fs;
ECG = abs(fft(ecg));
k = fs * (0:(slen/2 - 1)) / slen;

% Downsample
STEP = 5;
ecg_d = ecg(STEP:STEP:end);
t_d = (1:length(ecg_d))/(fs/STEP);
ECG_D = abs(fft(ecg_d));
k_d = (fs/STEP) * (0:(length(ecg_d)/2 - 1)) / length(ecg_d);

% Resample
ecg_r = resample(ecg, 1, STEP);
t_r = (1:length(ecg_r))/(fs/STEP);
ECG_R = abs(fft(ecg_r));
k_r = (fs/STEP) * (0:(length(ecg_r)/2 - 1)) / length(ecg_r);



figure
% Original
subplot(2, 3, 1);
plot(t, ecg);
axis tight;
xlabel('Time in seconds');
ylabel('ECG');
subplot(2, 3, 4);
plot(k, ECG(1:floor(length(ECG)/2)));
axis tight;
xlabel('Hz');
ylabel('Magnitude');

% Downsample
subplot(2, 3, 2);
plot(t_d, ecg_d);
axis tight;
xlabel('Time in seconds');
ylabel('Downsample ECG');
subplot(2, 3, 5);
plot(k_d, ECG_D(1:floor(length(ECG_D)/2)));
axis tight;
xlabel('Hz');
ylabel('Downsample Magnitude');

% Resample
subplot(2, 3, 3);
plot(t_r, ecg_r);
axis tight;
xlabel('Time in seconds');
ylabel('Resample ECG');
subplot(2, 3, 6);
plot(k_r, ECG_R(1:floor(length(ECG_R)/2)));
axis tight;
xlabel('Hz');
ylabel('Resample Magnitude');

