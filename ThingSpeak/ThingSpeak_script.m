
% Calculating channel
writeChannelID = 1636584;
% TODO - Enter the Write API Key between the '' below:
writeAPIKey = 'LA81E3TBW06I54EY';


cloud_value_scale = 30  % If the cloud value in detector never gets up to one in detector
                        % change this value to a lower value 
weight_557 = 1.75;       % Weighting factor when calculating aurora points in detector
weight_fraction = 150;  % Another weighting factor when calculating aurora points in detector
                        % If one use only values from the sensor with filter as alert factor
                        % set weight_557 to 1 and weight_fraction to 0
                        % and set limit for alert in IFTTT to something abowe 12 depending
                        % how eager you are

lat = 61.01014;         % Position of detector latitude
long = 14.600315;       % Position longitude
zenit_angle = 102;      % Position of sun for when it is dark enough. Note this is below horizon
UTCoff = 1;             % Coordinated Universal Time + / - 



% Calculation for the the night veto detarmain when it is night or not
days_in_year = 365;
c = datetime('today');
day_of_year = day(c, 'dayofyear');
g = gamma(day_of_year, days_in_year);
e = eqtime(g);
d = decl(g);
ha = hour_angle(lat,d,zenit_angle);
[dawn, dusk] = calc_dusk_dawn(day_of_year, days_in_year, lat, zenit_angle, long);
dawn = time_converter(dawn,c, UTCoff);
dusk = time_converter(dusk,c, UTCoff);
night_veto = time_veto(dawn,dusk);
%% Write Data %%
data = [night_veto, cloud_value_scale, weight_557, weight_fraction]
% Fields should be the same as in read_fields[] = {5,6,7,8} in aurora detector main
thingSpeakWrite(writeChannelID, 'Fields', [5,6,7,8],'Values', data, 'WriteKey', writeAPIKey);

function gamma = gamma(day_of_year, days_in_year)
    h=12;
    gamma = 2*pi*(day_of_year/days_in_year);
    
end
function eqtime = eqtime(gamma)
    eqtime = 229.18*(0.000075 + 0.001868*cos(gamma) - 0.032077*sin(gamma) - 0.014615*cos(2*gamma) - 0.040849*sin(2*gamma));
end
function decl = decl(gamma)
    decl = 0.006918 - 0.399912*cos(gamma) + 0.070257*sin(gamma) - 0.006758*cos(2*gamma) + 0.000907*sin(2*gamma) - 0.002697*cos(3*gamma) + 0.00148*sin(3*gamma);
end
function ha = hour_angle(lat, decl, zenit)
    zenit = to_rad(zenit);
    decl = to_rad(decl);
    lat = to_rad(lat);
    value = cos(zenit)/(cos(lat)*cos(decl)) - tan(lat)*tan(decl)
    if (value >= 1)
        ha = 0;
    elseif (value <= -1)
        ha = pi;
    else 
      ha = acos(value)  
    end
end
function rad = to_rad(deg)
    rad = deg/360 * 2 * pi;
end
function deg = to_deg(rad)
    deg = (rad/(2*pi)) * 360;
end
function [dawn, dusk] = calc_dusk_dawn(day_of_year, days_in_year, lat,zenit, longitude)
    g = gamma(day_of_year, days_in_year);
    e = eqtime(g);
    d = to_deg(decl(g));
    ha = to_deg(hour_angle(lat, d, zenit))
    
    dawn = 720 - 4*(longitude + ha) - e;
    dusk = 720 - 4*(longitude - ha) - e;
end
function time = time_converter(min,today, UTCoff)
    time = today;
    time = time + minutes(min);
    time = time + hours(UTCoff);
end
function veto = time_veto(dawn, dusk)
    if ( dawn < datetime('now') & dusk > datetime('now'))
        veto = 0;
    else
        veto = 1;
    end    
end
