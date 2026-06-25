# Derivative

- Learning event: `learn_2026_06_25t21_52_36z_derivative`
- Operational log: `kb/learning/logs/learn_2026_06_25t21_52_36z_derivative.json`
- Domain: `mathematics`
- Source: https://en.wikipedia.org/wiki/Derivative
- Wikipedia revision: `1358599947`
- Processed: `2026-06-25T21:52:36Z`

## Learned Concept

fundamental tool that quantifies the sensitivity to change of a function's output with respect to its input

## Extract

In mathematics, the derivative is a fundamental tool that quantifies the sensitivity to change of a function's output with respect to its input. The derivative of a function of a single variable at a chosen input value, when it exists, is the slope of the tangent line to the graph of the function at that point. The tangent line is the best linear approximation of the function near that input value. The derivative is often described as the instantaneous rate of change, the ratio of the instantaneous change in the dependent variable to that of the independent variable. The process of finding a derivative is called differentiation.

## Page Text

In mathematics, the derivative is a fundamental tool that quantifies the sensitivity to change of a function's output with respect to its input. The derivative of a function of a single variable at a chosen input value, when it exists, is the slope of the tangent line to the graph of the function at that point. The tangent line is the best linear approximation of the function near that input value. The derivative is often described as the instantaneous rate of change, the ratio of the instantaneous change in the dependent variable to that of the independent variable. The process of finding a derivative is called differentiation.

There are multiple different notations for differentiation. Leibniz notation, named after Gottfried Wilhelm Leibniz, is represented as the ratio of two differentials, whereas prime notation is written by adding a prime mark. Higher order notations represent repeated differentiation, and they are usually denoted in Leibniz notation by adding superscripts to the differentials, and in prime notation by adding additional prime marks. Higher order derivatives are used in physics; for example, the first derivative with respect to time of the position of a moving object is its velocity, and the second derivative is its acceleration.

Derivatives can be generalized to functions of several real variables. In this case, the derivative is reinterpreted as a linear transformation whose graph is (after an appropriate translation) the best linear approximation to the graph of the original function. The Jacobian matrix is the matrix that represents this linear transformation with respect to the basis given by the choice of independent and dependent variables. It can be calculated in terms of the partial derivatives with respect to the independent variables. For a real-valued function of several variables, the Jacobian matrix reduces to the gradient vector.

### Definition
#### As a limit
A function of a real variable f(x) is differentiable at a point a of its domain, if its domain contains an open interval containing , and the limit
L=\lim_{h \to 0}\frac{f(a+h)-f(a)}h
exists. This means that, for every positive real number , there exists a positive real number \delta such that, for every h such that |h| and h\ne 0 then f(a+h) is defined, and
\left|L-\frac{f(a+h)-f(a)}h\right|
where the vertical bars denote the absolute value. This is an example of the (, )-definition of limit.

If the function f is differentiable at , that is if the limit L exists, then this limit is called the derivative of f at . Multiple notations for the derivative exist. The derivative of f at a can be denoted , read as " prime of "; or it can be denoted , read as "the derivative of f with respect to x at " or " by (or over) dx at ". See below. If f is a function that has a derivative at every point in its domain, then a function can be defined by mapping every point x to the value of the derivative of f at . This function is written f' and is called the derivative function or the derivative of . The function f sometimes has a derivative at most, but not all, points of its domain. The function whose value at a equals f'(a) whenever f'(a) is defined and elsewhere is undefined is also called the derivative of . It is still a function, but its domain may be smaller than the domain of .

For example, let f be the squaring function: . Then the quotient in the definition of the derivative is
\frac{f(a+h) - f(a)}{h} = \frac{(a+h)^2 - a^2}{h} = \frac{a^2 + 2ah + h^2 - a^2}{h} = 2a + h.
The division in the last step is valid as long as . The closer h is to , the closer this expression becomes to the value . The limit exists, and for every input a the limit is . So, the derivative of the squaring function is the doubling function: .

The ratio in the definition of the derivative is the slope of the line through two points on the graph of the function , specifically the points (a,f(a)) and . As h is made smaller, these points grow closer together, and the slope of this line approaches the limiting value, the slope of the tangent to the graph of f at . In other words, the derivative is the slope of the tangent.

#### Using infinitesimals
One way to think of the derivative \frac{df}{dx}(a) is as the ratio of an infinitesimal change in the output of the function f to an infinitesimal change in its input. In order to make this intuition rigorous, a system of rules for manipulating infinitesimal quantities is required. The system of hyperreal numbers is a way of treating infinite and infinitesimal quantities. The hyperreals are an extension of the real numbers that contain numbers greater than anything of the form 1 + 1 + \cdots + 1 for any finite number of terms. Such numbers are infinite, and their reciprocals are infinitesimals. The application of hyperreal numbers to the foundations of calculus is called nonstandard analysis. This provides a way to define the basic concepts of calculus such as the derivative and integral in terms of infinitesimals, thereby giving a precise meaning to the d in the Leibniz notation. Thus, the derivative of f(x) becomes f'(x) = \operatorname{st}\left( \frac{f(x + dx) - f(x)}{dx} \right) for an arbitrary infinitesimal , where \operatorname{st} denotes the standard part function, which "rounds off" each finite hyperreal to the nearest real. Taking the squaring function f(x) = x^2 as an example again,
\begin{align}
f'(x) &= \operatorname{st}\left(\frac{x^2 + 2x \cdot dx + (dx)^2 -x^2}{dx}\right) \\
&= \operatorname{st}\left(\frac{2x \cdot dx + (dx)^2}{dx}\right) \\
&= \operatorname{st}\left(\frac{2x \cdot dx}{dx} + \frac{(dx)^2}{dx}\right) \\
&= \operatorname{st}\left(2x + dx\right) \\
&= 2x.
\end{align}

### Continuity and differentiability

If f is differentiable at , then f must also be continuous at . As an example, choose a point a and let f be the step function that returns the value 1 for all x less than , and returns a different value 10 for all x greater than or equal to . The function f cannot have a derivative at . If h is negative, then a + h is on the low part of the step, so the secant line from a to a + h is very steep; as h tends to zero, the slope tends to infinity. If h is positive, then a + h is on the high part of the step, so the secant line from a to a + h has slope zero. Consequently, the secant lines do not approach any single slope, so the limit of the difference quotient does not exist. However, even if a function is continuous at a point, it may not be differentiable there. For example, the absolute value function given by f(x) = |x| is continuous at , but it is not differentiable there. If h is positive, then the slope of the secant line from 0 to h is one; if h is negative, then the slope of the secant line from 0 to h is . This can be seen graphically as a "kink" or a "cusp" in the graph at . Even a function with a smooth graph is not differentiable at a point where its tangent is vertical: For instance, the function given by f(x) = x^{1/3} is not differentiable at . In summary, a function that has a derivative is continuous, but there are continuous functions that do not have a derivative.

Most functions that occur in practice have derivatives at all points or almost every point. Early in the history of calculus, many mathematicians assumed that a continuous function was differentiable at most points. Under mild conditions (for example, if the function is a monotone or a Lipschitz function), this is true. However, in 1872, Weierstrass found the first example of a function that is continuous everywhere but differentiable nowhere. This example is now known as the Weierstrass function. In 1931, Stefan Banach proved that the set of functions that have a derivative at some point is a meager set in the space of all continuous functions. Informally, this means that hardly any random continuous functions have a derivative at even one point.

### Notation

One common way of writing the derivative of a function is Leibniz notation, introduced by Gottfried Wilhelm Leibniz in 1675, which denotes a derivative as the quotient of two differentials, such as dy and . It is still commonly used when the equation y=f(x) is viewed as a functional relationship between dependent and independent variables. The first derivative is denoted by , read as "the derivative of y with respect to ". This derivative can alternately be treated as the application of a differential operator to a function, \frac{dy}{dx} = \frac{d}{dx} f(x). Higher derivatives are expressed using the notation \frac{d^n y}{dx^n} for the nth derivative of . These are abbreviations for multiple applications of the derivative operator; for example, \frac{d^2y}{dx^2} = \frac{d}{dx}\Bigl(\frac{d}{dx} f(x)\Bigr). Unlike some alternatives, Leibniz notation involves explicit specification of the variable for differentiation, in the denominator, which removes ambiguity when working with multiple interrelated quantities. The derivative of a composed function can be expressed using the chain rule: if u = g(x) and y = f(g(x)) then \frac{dy}{dx} = \frac{dy}{du} \cdot \frac{du}{dx}.

Another common notation for differentiation is by using the prime mark in the symbol of a function . This notation, due to Joseph-Louis Lagrange, is now known as prime notation. The first derivative is written as , read as " prime of ", or , read as " prime". Similarly, the second and the third derivatives can be written as f and , respectively. For denoting the number of higher derivatives beyond this point, some authors use Roman numerals in superscript, whereas others place the number in parentheses, such as f^{\mathrm{iv}} or . The latter notation generalizes to yield the notation f^{(n)} for the th derivative of .

In Newton's notation or the dot notation, a dot is placed over a symbol to represent a time derivative. If y is a function of , then the first and second derivatives can be written as \dot{y} and , respectively. This notation is used exclusively for derivatives with respect to time or arc length. It is typically used in differential equations in physics and differential geometry. However, the dot notation becomes unmanageable for high-order derivatives (of order 4 or more) and cannot deal with multiple independent variables.

Another notation is D-notation, which represents the differential operator by the symbol . The first derivative is written D f(x) and higher derivatives are written with a superscript, so the nth derivative is . This notation is sometimes called Euler notation, although it seems that Leonhard Euler did not use it, and the notation was introduced by Louis Francois Antoine Arbogast. To indicate a partial derivative, the variable differentiated by is indicated with a subscript, for example given the function , its partial derivative with respect to x can be written D_x u or . Higher partial derivatives can be indicated by superscripts or multiple subscripts, e.g. D_{xy} f(x,y) = \frac{\partial}{\partial y} \Bigl(\frac{\partial}{\partial x} f(x,y) \Bigr) and .

### Rules of computation

In principle, the derivative of a function can be computed from the definition by considering the difference quotient and computing its limit. Once the derivatives of a few simple functions are known, the derivatives of functions obtained from these simple functions by applying elementary operations, such as products, sums, quotients, or function composition, to them can be determined by applying rules for differentiation. This process of finding a derivative is called differentiation.

#### Rules for basic functions
The following are the rules for the derivatives of the most common basic functions. Here, a is a real number, and e is the base of the natural logarithm, approximately.
- Derivatives of powers:
- : \frac{d}{dx}x^a = ax^{a-1}

- Functions of exponential, natural logarithm, and logarithm with general base:
- : \frac{d}{dx}e^x = e^x
- : , for a > 0
- : , for x > 0
- : , for x, a > 0

- Trigonometric functions:
- : \frac{d}{dx}\sin(x) = \cos(x)
- : \frac{d}{dx}\cos(x) = -\sin(x)
- : \frac{d}{dx}\tan(x) = \sec^2(x) = \frac{1}{\cos^2(x)} = 1 + \tan^2(x)

- Inverse trigonometric functions:
- : , for -1
- : , for -1
- : \frac{d}{dx}\arctan(x)= \frac{1}

#### Rules for combined functions <span class="anchor" id="Rules"></span>
The following rules allow deducing derivatives of many functions from the derivatives of the basic functions:
- Constant rule: if f is a constant function, then for all ,
- : f'(x) = 0.
- Sum rule:
- : (\alpha f + \beta g)' = \alpha f' + \beta g' for all functions f and g and all real numbers \alpha and .
- Product rule:
- : (fg)' = f 'g + fg' for all functions f and . As a special case, this rule includes the fact (\alpha f)' = \alpha f' whenever \alpha is a constant because \alpha' f = 0 \cdot f = 0 by the constant rule.
- Quotient rule:
- : \left(\frac{f}{g} \right)' = \frac{f'g - fg'}{g^2} for all functions f and g at all inputs where .
- Chain rule for composite functions: If , then
- : f'(x) = h'(g(x)) \cdot g'(x).

#### Computation example
The derivative of the function given by f(x) = x^4 + \sin \left(x^2\right) - \ln(x) e^x + 7 is
\begin{align}
f'(x) &= 4 x^{(4-1)}+ \frac{d\left(x^2\right)}{dx}\cos \left(x^2\right) - \frac{d\left(\ln {x}\right)}{dx} e^x - \ln(x) \frac{d\left(e^x\right)}{dx} + 0 \\
&= 4x^3 + 2x\cos \left(x^2\right) - \frac{1}{x} e^x - \ln(x) e^x.
\end{align}
Here the second term was computed using the chain rule and the third term using the product rule. The known derivatives of the elementary functions , , , , and , as well as the constant , were also used.

### Antidifferentiation

An antiderivative of a function f is a function whose derivative is . Antiderivatives are not unique: if A is an antiderivative of , then so is , where c is any constant, because the derivative of a constant is zero. The fundamental theorem of calculus shows that finding an antiderivative of a function gives a way to compute the areas of shapes bounded by that function. More precisely, the integral of a function over a closed interval is equal to the difference between the values of an antiderivative evaluated at the endpoints of that interval.

### Higher-order derivatives<span class="anchor" id="order of derivation"></span><span class="anchor" id="Order"></span>
Higher-order derivatives are the result of differentiating a function repeatedly. Given that f is a differentiable function, the derivative of f is the first derivative, denoted as . The derivative of f' is the second derivative, denoted as , and the derivative of f is the third derivative, denoted as . By continuing this process, if it exists, the th derivative is the derivative of the th derivative or the derivative of order . As has been discussed above, the generalization of derivative of a function f may be denoted as . A function that has k successive derivatives is called k times differentiable. If the th derivative is continuous, then the function is said to be of differentiability class . A function that has infinitely many derivatives is called infinitely differentiable or smooth. Any polynomial function is infinitely differentiable; taking derivatives repeatedly will eventually result in a constant function, and all subsequent derivatives of that function are zero.

One application of higher-order derivatives is in physics. For example, if the function represents an object's position with respect to time, represents the object's velocity, represents the object's acceleration, and represents the object's jerk.

### In other dimensions

#### Vector-valued functions
A vector-valued function \mathbf{y} of a real variable sends real numbers to vectors in some vector space . A vector-valued function can be split up into its coordinate functions , meaning that . This includes, for example, parametric curves in \R^2 or . The coordinate functions are real-valued functions, so the above definition of derivative applies to them. The derivative of \mathbf{y}(t) is defined to be the vector, called the tangent vector, whose coordinates are the derivatives of the coordinate functions. That is,
\mathbf{y}'(t)=\lim_{h\to 0}\frac{\mathbf{y}(t+h) - \mathbf{y}(t)}{h},
if the limit exists. The subtraction in the numerator is the subtraction of vectors, not scalars. If the derivative of \mathbf{y} exists for every value of , then \mathbf{y}' is another vector-valued function.

#### Partial derivatives

Functions can depend upon more than one variable. A partial derivative of a function of several variables is its derivative with respect to one of those variables, with the others held constant. Partial derivatives are used in vector calculus and differential geometry. As with ordinary derivatives, multiple notations exist: the partial derivative of a function f(x, y, \dots) with respect to the variable x is variously denoted by

among other possibilities. It can be thought of as the rate of change of the function in the x-direction. Here [[]] is a rounded d called the partial derivative symbol. To distinguish it from the letter d, is sometimes pronounced "der", "del", or "partial" instead of "dee". For example, let , then the partial derivative of function f with respect to both variables x and y are, respectively:
\frac{\partial f}{\partial x} = 2x + y, \qquad \frac{\partial f}{\partial y} = x + 2y.
In general, the partial derivative of a function f(x_1, \dots, x_n) in the direction x_i at the point (a_1, \dots, a_n) is defined to be:
\frac{\partial f}{\partial x_i}(a_1,\ldots,a_n) = \lim_{h \to 0}\frac{f(a_1,\ldots,a_i+h,\ldots,a_n) - f(a_1,\ldots,a_i,\ldots,a_n)}{h}.

This is fundamental for the study of the functions of several real variables. Let f(x_1, \dots, x_n) be such a real-valued function. If all partial derivatives f with respect to x_j are defined at the point , these partial derivatives define the vector
\nabla f(a_1, \ldots, a_n) = \left(\frac{\partial f}{\partial x_1}(a_1, \ldots, a_n), \ldots, \frac{\partial f}{\partial x_n}(a_1, \ldots, a_n)\right),
which is called the gradient of f at . If f is differentiable at every point in some domain, then the gradient is a vector-valued function \nabla f that maps the point (a_1, \dots, a_n) to the vector . Consequently, the gradient determines a vector field.

#### Directional derivatives

If f is a real-valued function on , then the partial derivatives of f measure its variation in the direction of the coordinate axes. For example, if f is a function of x and , then its partial derivatives measure the variation in f in the x and y direction. However, they do not directly measure the variation of f in any other direction, such as along the diagonal line . These are measured using directional derivatives. Given a vector , then the directional derivative of f in the direction of \mathbf{v} at the point \mathbf{x} is:
D_{\mathbf{v}}{f}(\mathbf{x}) = \lim_{h \rightarrow 0}{\frac{f(\mathbf{x} + h\mathbf{v}) - f(\mathbf{x})}{h}}.

If all the partial derivatives of f exist and are continuous at , then they determine the directional derivative of f in the direction \mathbf{v} by the formula:
D_{\mathbf{v}}{f}(\mathbf{x}) = \sum_{j=1}^n v_j \frac{\partial f}{\partial x_j}.

#### Total derivative and Jacobian matrix

When f is a function from an open subset of \R^n to , then the directional derivative of f in a chosen direction is the best linear approximation to f at that point and in that direction. However, when , no single directional derivative can give a complete picture of the behavior of . The total derivative gives a complete picture by considering all directions at once. That is, for any vector \mathbf{v} starting at , the linear approximation formula holds:
f(\mathbf{a} + \mathbf{v}) \approx f(\mathbf{a}) + f'(\mathbf{a})\mathbf{v}.
Similarly with the single-variable derivative, f'(\mathbf{a}) is chosen so that the error in this approximation is as small as possible. The total derivative of f at \mathbf{a} is the unique linear transformation f'(\mathbf{a}) \colon \R^n \to \R^m such that
\lim_{\mathbf{h}\to 0} \frac{\lVert f(\mathbf{a} + \mathbf{h}) - (f(\mathbf{a}) + f'(\mathbf{a})\mathbf{h})\rVert}{\lVert\mathbf{h}\rVert} = 0.
Here \mathbf{h} is a vector in , so the norm in the denominator is the standard length on . However, f'(\mathbf{a}) \mathbf{h} is a vector in , and the norm in the numerator is the standard length on . If v is a vector starting at , then f'(\mathbf{a}) \mathbf{v} is called the pushforward of \mathbf{v} by .

If the total derivative exists at , then all the partial derivatives and directional derivatives of f exist at , and for all , f'(\mathbf{a})\mathbf{v} is the directional derivative of f in the direction . If f is written using coordinate functions, so that , then the total derivative can be expressed using the partial derivatives as a matrix. This matrix is called the Jacobian matrix of f at \mathbf{a} :
f'(\mathbf{a}) = \operatorname{Jac}_{\mathbf{a}} = \left(\frac{\partial f_i}{\partial x_j}\right)_{ij}.

### Generalizations

The concept of a derivative can be extended to many other settings. The common thread is that the derivative of a function at a point serves as a linear approximation of the function at that point.
- An important generalization of the derivative concerns complex functions of complex variables, such as functions from (a domain in) the complex numbers \C to . The notion of the derivative of such a function is obtained by replacing real variables with complex variables in the definition. If \C is identified with \R^2 by writing a complex number z as then a differentiable function from \C to \C is certainly differentiable as a function from \R^2 to \R^2 (in the sense that its partial derivatives all exist), but the converse is not true in general: the complex derivative only exists if the real derivative is complex linear and this imposes relations between the partial derivatives called the Cauchy--Riemann equations -- see holomorphic functions.
- Another generalization concerns functions between differentiable or smooth manifolds. Intuitively speaking such a manifold M is a space that can be approximated near each point x by a vector space called its tangent space: the prototypical example is a smooth surface in . The derivative (or differential) of a (differentiable) map f:M\to N between manifolds, at a point x in , is then a linear map from the tangent space of M at x to the tangent space of N at . The derivative function becomes a map between the tangent bundles of M and . This definition is used in differential geometry.
- Differentiation can also be defined for maps between vector space, such as Banach space, in which those generalizations are the Gateaux derivative and the Frechet derivative.
- One deficiency of the classical derivative is that very many functions are not differentiable. Nevertheless, there is a way of extending the notion of the derivative so that all continuous functions and many other functions can be differentiated using a concept known as the weak derivative. The idea is to embed the continuous functions in a larger space called the space of distributions and only require that a function is differentiable "on average".
- Properties of the derivative have inspired the introduction and study of many similar objects in algebra and topology; an example is differential algebra. Here, it consists of the derivation of some topics in abstract algebra, such as rings, ideals, field, and so on.
- The discrete equivalent of differentiation is finite differences. The study of differential calculus is unified with the calculus of finite differences in time scale calculus.
- The arithmetic derivative involves the function that is defined for the integers by the prime factorization. This is an analogy with the product rule.
- Derivations generalize derivatives to algebraic settings, such as rings.

### See also
- Covariant derivative
- Derivation
- Exterior derivative
- Functional derivative
- Implicit differentiation
- Lie derivative

### Notes

### References

-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
- . See the English version here.
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-
-

### External links

-
- Khan Academy: "Newton, Leibniz, and Usain Bolt"
-
- Online Derivative Calculator from Wolfram Alpha.
