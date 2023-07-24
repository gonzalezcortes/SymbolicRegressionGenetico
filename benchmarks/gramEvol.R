#benchmark
#https://blog.ephorie.de/symbolic-regression-genetic-programming-or-if-kepler-had-r
#install.packages("gramEvol")
library("gramEvol")

test_1 <- function(){
  set.seed(314)
  start.time <- Sys.time()
  x <- seq(0, 4*pi, length.out = 201)
  y <- jitter(sin(x) + cos(x + x), amount = 0.2)
  
  
  
  ruleDef <- list(expr = grule(op(expr, expr), func(expr), var),
                  func = grule(sin, cos, exp),
                  op = grule('+', '-', '*', '/'),
                  var = grule(x,1,2,3))
  
  grammarDef <- CreateGrammar(ruleDef)
  
  SymRegFitFunc <- function(expr) {
    result <- eval(expr)
    if (any(is.nan(result)))
      return(Inf)
    return (mean(log(1 + abs(y - result))))
  }
  
  ge <- GrammaticalEvolution(grammarDef, SymRegFitFunc, terminationCost = 0.1, iterations = 10000, max.depth = 3)
  print(ge)
  print(Sys.time() - start.time)
  
  return (ge)
}

ge <- test_1()

######## plot ################
x <- seq(0, 4*pi, length.out = 201)
y <- jitter(sin(x) + cos(x + x), amount = 0.2)
plot(y)
points(eval(ge$best$expressions), col = "red", type = "l")


