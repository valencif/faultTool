%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This script plots the evolution and normal correlations contained in a
% folder. the data is saved in file which name starts with corr, every line
% corresponds to the evolution of the correlation of one given key
%

function Plot(resultsFolder, figuresPath, correctKey, numTests)


close all;



%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Global variables

% resultsFolder = './VAR/PAR/';
% figuresPath = './FIG/PAR/';
% correctKey = 103;

prefix='Corr';

mkdir(figuresPath)

labelFontSize = 24;
lineWidth = 1.5;
markerSize = 8;


%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Get list of correlation files

files = dir(resultsFolder);
filesNum = size(files,1);
prefixLen = length(prefix);

fileCorrelation = {};
modelNames = {};


for i = 1:1:filesNum
    file=files(i).name;
    if( size(file, 2) > size(prefix,2) )
        if( strcmp(file(1:prefixLen),prefix)  )
            fileCorrelation = [ fileCorrelation ; file];
            modelNames = [modelNames ; file( prefixLen+1 : end-4)];
        end
    end    
end 



%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  Plot evolution and correlation for every model

filesNum = size(fileCorrelation,1);

for i = 1:1:filesNum
    disp( ['Reading file' resultsFolder  fileCorrelation{i}] );
    filename= [resultsFolder fileCorrelation{i} ];
    corrEvoM = dlmread(filename);
    plotpoints = size(corrEvoM,2);
    testAxis = (1:1:plotpoints)*numTests/plotpoints;
    corrVec = corrEvoM(:,end);
    kevEvo = corrEvoM(correctKey+1,:);
    corrEvoM(correctKey+1,:)=[];
    fig1 = figure;
    set(fig1, 'Position', get(0,'Screensize')); 
    plot(testAxis,abs(corrEvoM)','b','LineWidth',lineWidth)
    grid on
    hold on
    plot(testAxis,abs(kevEvo), '-*r','LineWidth',lineWidth,'MarkerSize',markerSize)
    title(['Correlation Evolution ' modelNames{i}], 'FontSize', labelFontSize+4)
    xlabel('Number of ciphertexts','FontSize',labelFontSize)
    ylabel('Correlation','FontSize',labelFontSize)
    xlim([testAxis(1) testAxis(end)])
    ax=gca;
    set(ax,'FontSize',labelFontSize-4);
    
    savefig(fig1, [figuresPath 'Evo' modelNames{i} '.fig'])
    saveas(fig1, [figuresPath 'Evo' modelNames{i} '.png']);
    
    % %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    fig2 = figure;
    set(fig2, 'Position', get(0,'Screensize')); 
    keys = 0:1:length(corrVec)-1;
    hPlot = plot(keys, abs(corrVec),'LineWidth',lineWidth);
    grid on
    cursorMode = datacursormode(fig2);
    
    title(['Correlation ' modelNames{i}], 'FontSize', labelFontSize+4)
    xlabel('Number of ciphertexts','FontSize',labelFontSize)
    ylabel('Correlation','FontSize',labelFontSize)
    ylim([0 1]);
    xlim([keys(1) keys(end)])
    ax=gca;
    set(ax,'FontSize',labelFontSize-4);
    
    hDatatip = cursorMode.createDatatip(hPlot);
    set(hDatatip, 'MarkerSize',markerSize+4, 'MarkerFaceColor','none',...
                  'MarkerEdgeColor','r', 'OrientationMode','auto')
    pos=[correctKey corrVec(correctKey+1)];
    set(hDatatip, 'Position', pos) 
    
    savefig(fig2, [figuresPath 'Corr' modelNames{i} '.fig'])
    saveas(fig2, [figuresPath 'Corr' modelNames{i} '.png']);
    
    close all;
    
end





