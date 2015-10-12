#!/usr/bin/Rscript

# James Jessen
# Data Science
# Project #1 - EDA

# Usage:
#------------------------------------------------------------------------------
# R
# source("graph.R", print.eval=TRUE)
#==============================================================================

#install.packages("ggplot2", repos="http://cran.us.r-project.org")
library(ggplot2)

doPlot <- TRUE

# read data
cat("Reading input file...")
sensor_data <- read.csv("processed.csv")
cat("Done!\n")

sensor_data <- sensor_data[with(sensor_data, order(Movement)),]

movements <- c("sit_hands_lap", "sit_write", "sit_type", "lying_back", "lying_side", "jumping_jacks", "butt_kickers", "high_knees", "flutter_kicks", "lunges", "cruches", "squats", "pushups", "walk_upstairs", "walk_downstairs", "stand_arms_side", "stand_arms_cross", "slow_walk", "normal_walk", "fast_walk", "jog", "run", "bike", "dips")

nodes <- c("head", "chest", "backpack", "right_arm", "left_thigh", "left_ankle", "right_ankle", "left_wrist", "right_wrist", "waist")

# Map movement number to description 
sensor_data$Movement <- factor(sensor_data$Movement, levels = 1:24, labels = movements)

# Map node number to description
sensor_data$Node <- factor(sensor_data$Node, levels = 1:10, labels = nodes) 

fields <- colnames(sensor_data)
sensors <- fields[1:12]

# view
#head(sensor_data)
#summary(sensor_data)

require(ggplot2)
for(mvt in movements) 
{
    # Show progress
    seperater <- paste0("------------------------------------------ Movement: ", mvt, " ------------------------------------------\n")
    cat(seperater)

    for(node in nodes)
    {
        # Select a movement and node 
        plot_data <- subset(plot_data, Movement == mvt & Node == Node)

        # Each file is for a movement/node combination
        out.file <- paste0("output/", mvt, "-", node,".pdf")
        pdf(out.file, height=5, width=100)

        for(sensor in sensors) 
        {
            # Each page of a file has a different sensor 
            # Each line on a graph is for a different subject
            plot <- ggplot(data=plot_data, aes_string(x="Time", y=sensor, color="Subject")) +
                ggtitle(sensor) +
                labs(x="", y="") +
                scale_x_continuous(expand = c(0, 0)) + 
                geom_line(size=0.1)
            print(plot)
        }
        dev.off()
    }
    stop("STOPPED")
}
