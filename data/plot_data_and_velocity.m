% Gregary C. Zweigle, 2022

% Samples of Renner action hammer motion.
% Keys are in the 4th octave but I don't recall
% exactly which keys were struck to create the data.
% This data was taken about 6 months ago.

% Data is normalized to 1.0 as max ADC range.
% Variation in amplitude is due to non-uniform sensor placement.

% ADC is 24-bit but data was heavily post-quantized
% due to a temporary implementation constraint.
% This has been fixed and future data sets can be full 24-bit.

% Data was sampled at 8000 samples per second.

% The following Matlab file demonstrates analysis of 
% position and velocity.  This code was written quickly,
% it can be improved.  Note that the point where hammer
% flies off jack toward string is when the velocity peaks
% and starts to decrease. When the hammer hits the string
% the velocity is zero.

clear;

load samples.txt;
len = size(samples)(1);

sample_rate = 8000;
time = [0:len-1]/sample_rate;

% Back of the envelope velocity computation:

% Derivative filter to compute velocity.
% The filter is [1 -1] convolved with a boxcar filter.
% The boxcar is to remove some noise.
M = 10;
b = [1 zeros(1,M) -1];
D = round(length(b)/2);  % Filter delay in units of samples.

% Velocity estimate in meters per second.
% Units: The hammer moves approximately 5/16 of an inch when
% traveling the normalized distance (0 to 1 on vertical axis).
% (5/16 inch * .0254 inches/meter) = the distance covered in meters.
% Velocity can be sanity checked by dx/dt with a ruler on the plots.
dx = filter(b,[1],samples) * 5/16 * 0.0254 * sample_rate / (M+2);
% Remove the filter initialization transient.
dx(1:length(b),:) = zeros(length(b),size(dx)(2));

% Plot the hammer positions with respect to time.
figure(1);
plot(time,samples);
title('Renner action hammer samples');
xlabel('time, seconds');
ylabel('position, normalized');
grid;

% Plot the hammer position and an approximation of the velocity
% with respect to time.  Use the delay value (D) to shift the
% velocity curves in time. This removes the filter delay and
% enables comparing hammer position with associated velocity.
figure(2);
subplot(2,2,1);
plot(time,samples(:,1),time(1:end-D),dx(D+1:end,1));
xlabel('time, seconds');
ylabel('position and velocity (m/s)');
grid;
subplot(2,2,2);
plot(time,samples(:,2),time(1:end-D),dx(D+1:end,2));
xlabel('time, seconds');
ylabel('position and velocity (m/s)');
grid;
subplot(2,2,3);
plot(time,samples(:,3),time(1:end-D),dx(D+1:end,3));
xlabel('time, seconds');
ylabel('position and velocity (m/s)');
grid;
subplot(2,2,4);
plot(time,samples(:,4),time(1:end-D),dx(D+1:end,4));
xlabel('time, seconds');
ylabel('position and velocity (m/s)');
grid;