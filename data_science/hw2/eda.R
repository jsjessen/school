# move into data directory
setwd('data')

#days <- 1:31

#sprintf("nyt%d.csv", days[1], filename)
#data1 <- read.csv(filename)
data1 <- read.csv("nyt1.csv")

#names(data1)
colnames(data1) <- c('age', 'gender', 'views', 'clicks', 'signed')
str(data1)
summary(data1)
dim(data1)


# I may also be interested in age. Rather than looking at it 
#  continuously, I decide to create age groups: young adults,
#  adults, and older adults
# First, initialize a new variable:
#age_group <- rep(NA, 458441) #number of rows in dataset
#age_group[data1$age < 18] <- "<18"
#age_group[data1$age >= 18 & data1$age <=24] <- "18-24"
#age_group[data1$age >= 25 & data1$age <=40] <- "25-40" 
#age_group[data1$age >= 41 & data1$age <=64] <- "41-64"
#age_group[data1$age >= 65] <- "65+" 
## Categorize and attach to your dataset
#data1$age_group <- as.factor(age_group) 

#data1$age_group <- factor(data1$age_group, levels=0:18, labels= "<18")
#data1$age_group <- factor(data1$age_group, levels=0:18, labels= "<18")

data1$age_group <- as.character(cut(data1$age, breaks=c(-1,18,24,40,64,Inf),
                           labels=c('<18', '18-24', '25-40', '41-64', '65+')))

data1$CTR <- data1$clicks / data1$views
head(data1, 10)
summary(data1)
str(data1)

plot(data1[c(age_group, CTR)])

#CTR = clicks / views
