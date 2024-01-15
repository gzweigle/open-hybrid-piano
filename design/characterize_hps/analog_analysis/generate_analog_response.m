% Copyright (C) 2024 Greg C. Zweigle
%
% This program is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program. If not, see <https://www.gnu.org/licenses/>.
%
% Location of documentation, code, and design:
% https://github.com/gzweigle/DIY-Grand-Digital-Piano
%
% generate_analog_response.m
%
% Run from within Matlab or Octave.
%
clear;

printf("Reading data file...\n");

% Hammer data in ADC counts.
% get_hammer_cal_data.py saves results into hammer_cal_data.txt.
% Then, file is manually renamed to a file name that describes the test.
% Use the renamed name here:
load ../data/hammer_bf2_500us_iter5.txt;
% Only first column of data received from stem piano (Teensy 4.1) is needed.
adc = hammer_bf2_500us_iter5(:,1);

% Write results location.
% This filename used by combine_analog_responses.m.
fid = fopen("../data/cny5.txt","wt");

% Sample period, from stem piano settings file.
T = 300e-6;

% When variance exceeds the nominal value plus this margin,
% the code below estimates that the ADC values are undergoing
% a change from the previous hold value to the next hold value.
% The var_threshold margin is determined by looking at plots.
var_threshold = 1000.0;

% Lengths.
N = length(adc);
t = [0:N-1]*T;

% How long the linear actuator holds at each level.
% The value of t_hold must match the value in hammer_crane.cpp.
t_hold = 0.5;
N_hold_float = t_hold/T;

% How many hold values to average together.
% Make smaller than the full hold time because want
% to avoid catching trasient data at the edges of
% the hold time.
N_avg = round(N_hold_float/2);

% How long it takes for the actuator to switch between levels.
% The value of t_tr must match the value in hammer_crane.cpp.
t_tr = 10e-3;
N_tr = round(t_tr/T);

% Window over which to compute variance of adc values.
% Shorter than hold time to avoid transition effects.
N_var = round(N_hold_float/5);

% To find the transition point, step the variance window
% by some fraction of transient time. The offset is to
% remove the variance window delay when aligning to adc.
N_tr_step = round(N_tr/5);
N_tr_off = round(N_var/2);

printf("Finding each hold region in the data...\n");

% Find the actuator transitions between hold values by computing
% adc variance and then taking the peak variance as the transition.
var_array = zeros(1,N);
for n = 1:N_tr_step:N-N_var-1,
  % Step through date by N_tr_step.
  % At each step, compute variance over window of size N_var.
  var_array(n+N_tr_off:n+N_tr_step+N_tr_off) = var(adc(n:n+N_var));
end;
% First variance is the basedline. This assumes that the
% hammer starts in the rest position, which is always the case.
var_baseline = var_array(1+N_tr_off);
var_baseline = var_baseline + var_threshold;

printf("Marking the start of each hold region in the data...\n");

% The variance around actuator transitions looks like a bell-shaped
% curve. Find the max of each bell-shaped curve and mark that as
% the start of a linear actuator hold time.
samp_points = zeros(1,N);
n = 1;
done = 0;
% Code could be more efficient but keeping it verbose so
% that it is easier to remember what this code is doing!
while done == 0,
  if var_array(n) > var_baseline,
    % Found the start of a bell-shaped variance curve around the
    % transition point. Find the max and then shift it by 5*N_tr
    % to approximately place a marker at the start of hold time.
    [m, im] = max(var_array(n:n+N_var));
    samp_points(n+im+5*N_tr) = 1;
    % Skip past present bell-shaped variance curve to avoid
    % double counting.
    n = n + 2*N_var;
  else
    % Didn't find the start of a transition so just increment.
    n = n + 1;
  end;
  % Not using a for loop because of need to change the loop
  % value n, as shown below. Hence, the while() loop.
  if n > N - 2*N_var,
    done = 1;
  end;
end;

printf("Generating response curves...\n");

% Now get the CNY7+HPS response by averaging within a hold time.
ind = 1;
cny_array = zeros(1,N);
for n = 1:N-N_avg,
  if samp_points(n) == 1,
    cny(ind) = mean(adc(n:n+N_avg));  % Result of this code.
    cny_array(n:n+N_avg) = cny(ind);  % Use for plotting.
    ind = ind + 1;
  end;
end;
cny = cny / max(cny);

figure(1);
subplot(2,1,1);
% As a check on all algorithms, plot the following:
% adc = original data.
% samp_points = place where cny values are calculated.
% cny_array = sample-hold of cny averaged values.
% var_array = variance, should be bell-shaped at each transition.
% Should see cny_array along the average of adc values.
plot(1:N,adc,1:N,50000*samp_points,1:N,var_array,1:N,cny_array);
grid;
subplot(2,1,2);
% This is the cny curve we are after, along with the linear ideal.
plot(1:length(cny),cny,[1 length(cny)],[cny(1) cny(end)]);
grid;

% Save to a file.
fprintf(fid,"%f\n",cny);