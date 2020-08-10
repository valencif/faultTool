%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This script plots the generated templates, the data is saved in files 
% which finish in 'Template.txt'
%

function PlotTemplate(resultsFolder,figuresPath )


close all;

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Global variables

% resultsFolder = './VAR/PAR/';
% figuresPath = './FIG/PAR/';

suffix = 'Template.txt';
nonPrefix = {'Corr'; 'Sen'};

mkdir(figuresPath)

labelFontSize = 24;
lineWidth = 1.5;
markerSize = 8;


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Get list of correlation files

files = dir(resultsFolder);
filesNum = size(files,1);
suffixLen = length(suffix);

fileTemplate = {};
templatesNames = {};


for i = 1:1:filesNum
    file=files(i).name;
    if( length(file) > suffixLen )
        if( strcmp(file(end-suffixLen+1:end),suffix)  )
            fileTemplate = [ fileTemplate ; file];
        end
    end    
end 


filesNum = length(fileTemplate);

for p=1:1:length(nonPrefix)
    NPrefixLen = length(nonPrefix{p});
    for i = 1:1:filesNum
        file = fileTemplate{i};
        if( length(file) > NPrefixLen )
            if( strcmp(file(1:NPrefixLen),nonPrefix{p})  )
                fileTemplate{i} = [];
            end
        end    
    end
end

fileTemplate = fileTemplate(~cellfun('isempty',fileTemplate));


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Plot templates

filesNum = length(fileTemplate);
for t=1:1:filesNum
    filename= [resultsFolder fileTemplate{t} ];
    templateV = dlmread(filename);
    
    fig1 = figure;
    set(fig1, 'Position', get(0,'Screensize')); 
    intermediates = 0:1:length(templateV)-1;
    plot(intermediates, templateV,'LineWidth',lineWidth)
    grid on
    hold on
    title(['Template ' fileTemplate{t}], 'FontSize', labelFontSize+4)
    xlabel('Intermediate values','FontSize',labelFontSize)
    ylabel('Sensitivity','FontSize',labelFontSize)
    xlim([intermediates(1) intermediates(end)]);
    ax=gca;
    set(ax,'FontSize',labelFontSize-4);
    
    savefig(fig1, [figuresPath 'Temp' fileTemplate{t} '.fig'])
    saveas(fig1, [figuresPath 'Temp' fileTemplate{t} '.png']);

    close all;
end






