#!/usr/bin/Rscript

install.packages("nutshell", repos="http://cran.us.r-project.org")
library(nutshell)
data(team.batting.00to08)
attach(team.batting.00to08)
head(startdata)

help(lm)

runs.mdl <- lm (formula =
                runs ~ singles + doubles + triples + homeruns + walks + hitbypitch + sacrificeflies + stolenbases + caughtstealing,
                data = team.batting.00to08)
summary(runs.mdl)
formula(runs.mdl)
coef(runs.mdl)
plot(runs.mdl)

# page 80 of the book i think
help(rnorm)
x_1 <- rnorm(1000,5,7)
hist(x_1, col="grey")
true_error = rnorm(1000,0,2)
true_beta_0 <- 1.1
true_beta_1 <- -8.2 
y <- true_beta_0 + true_beta_1*x_1 + true_error
hist(y)
plot(x_1, y, pch=20, col="red")

model1 <- lm(y ~ x_1)
coef(model1)

# normal distribution looks like a perfect hill, centered
# gamma distribution looks like just the slop for downhill skii
help(rgamma)
x_2 <- rgamma(1000,1,1)

model2_1 <- lm(y ~ x_2)
hist(model2_1)
summary(model2_1)
coef(model2_1)
y2 <- true_beta_0 + true_beta_1*x_1 + 0.4*x_2

model2_2 <- lm(y2 ~ x_1 + x_2)
coef(model2_2)
