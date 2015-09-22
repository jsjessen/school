#!/usr/bin/Rscript

# James Jessen
# Data Science
# Project #1 - EDA

# Usage:
#------------------------------------------------------------------------------
# R
# source("sample.R", print.eval=TRUE))
#==============================================================================

#install.packages("doBy", repos="http://cran.us.r-project.org")
library("doBy")

#install.packages("ggplot2", repos="http://cran.us.r-project.org")
library(ggplot2)

doPlot <- TRUE 

clen <- function(x){c(length(x))}
siterange <- function(x){c(length(x), min(x), mean(x), max(x))}

#data1 <- list()
days <- 31
for(day in 1:days)
{
    # read data
    in.file <- paste0("data/nyt",day,".csv")
    data1 <- read.csv(in.file)

    # categorize
    head(data1)
    data1$age_group <- cut(data1$Age,c(-Inf,17,24,40,64,Inf),
                           labels=c("<18", "18-24", "25-40", "41-64", "65+"))

    # label and factor gender
    data1$Gender[data1$Gender == 0] <- "Female"
    data1$Gender[data1$Gender == 1] <- "Male"
    data1$Gender <- factor(data1$Gender)

    # factor sign-in
    data1$Signed_In <- factor(data1$Signed_In)

    # create categories
    data1$scode[data1$Impressions == 0] <- "NoImps"
    data1$scode[data1$Impressions > 0] <- "Imps"
    data1$scode[data1$Clicks > 0] <- "Clicks"
    data1$scode <- factor(data1$scode)

    #look at counts 
    etable <- summaryBy(Impressions~scode+Gender+age_group, data=data1, FUN=clen)

    # view
    head(data1)
    summary(data1)

    # brackets
    cat("******\n")
    sum <- summaryBy(Age~age_group, data=data1, FUN=siterange)
    colnames(sum) <- c("Age Group", "Population", "Min", "Mean", "Max")
    sum

    # so only signed in users have ages and genders
    cat("******\n")
    summaryBy(Gender+Signed_In+Impressions+Clicks~age_group, data=data1)

    # create click thru rate
    # we don't care about clicks if there are no impressions
    # if there are clicks with no imps my assumptions about this data are wrong
    data1$hasimps <- cut(data1$Impressions,c(-Inf,0,Inf))
    cat("******\n")
    sum <- summaryBy(Clicks~hasimps, data=data1, FUN=siterange)
    colnames(sum) <- c("hasimps", "Clicks", "Min", "Mean", "Max")
    sum

    if(doPlot)
    {
        # plot
        
        out.file <- paste0("output/charts_day",day,".pdf")
        pdf(out.file)
        
        ggplot(data1, aes(x=Clicks/Impressions,fill=age_group)) +
            ggtitle("Click Through Rate (CTR): Ages") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Age Group") +
            geom_histogram(binwidth=0.05)
        
        ggplot(subset(data1, Impressions>0 & Clicks>0), 
            aes(x=Clicks/Impressions,fill=age_group)) +
            ggtitle("CTR Seen & Clicked: Ages") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Age Group") +
            geom_histogram(binwidth=0.05)
        
        ggplot(subset(data1, 
            (Signed_In==1 & age_group=="<18") & 
            Impressions>0 & Clicks>0), 
            aes(x=Clicks/Impressions,fill=Gender)) +
            ggtitle("CTR Seen & Clicked: Females vs Males under 18 years old") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Gender") +
            geom_histogram(binwidth=0.05)
        
        ggplot(subset(data1, 
            (Signed_In==1 & Gender=="Female") & (age_group=="18-24" | age_group=="25-40") & 
            Impressions>0 & Clicks>0), 
            aes(x=Clicks/Impressions,fill=age_group)) +
            ggtitle("CTR Seen & Clicked: Females 18-24 vs 25-40 years old") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Age Group") +
            geom_histogram(binwidth=0.05)
        
        ggplot(subset(data1, 
            (Signed_In==1 & Gender=="Male") & (age_group=="18-24" | age_group=="25-40") & 
            Impressions>0 & Clicks>0), 
            aes(x=Clicks/Impressions,fill=age_group)) +
            ggtitle("CTR Seen & Clicked: Males 18-24 vs 25-40 years old") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Age Group") +
            geom_histogram(binwidth=0.05)
        
        ggplot(subset(data1,Impressions>0 & Clicks>0), 
            aes(x=Clicks/Impressions,fill=Signed_In)) +
            ggtitle("CTR Seen & Clicked: Signed-In vs Signed-Out") +
            labs(x="Clicks / Impressions",y="Count") +
            scale_fill_discrete(name="Signed",labels=c("Out", "In")) +
            geom_histogram(binwidth=0.05)
        
        dev.off()
    }

    seperater <- paste0("\n------------------------------------- Day ",day," -------------------------------------\n")
    cat(seperater)
}

