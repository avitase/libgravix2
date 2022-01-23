The propagation of point-like particles, aka _missiles_, are simulated in the gravitational force field of static center of masses, aka _planets_, on the surface of a unit-sphere.
%Planets and missiles are restricted to stay exactly on the surface at any time.

In this document we describe in detail the theoretical background and list all equations that are necessary to efficiently deal with this library.
If you prefer a more experimental orientated approach you can skip this section and directly jump to [the description of the API](@ref include/api.h).
There, you will find everything necessary to use the library and to run your simulations.

# Theory
%Planets generate a static gravitational force field \f$\vec\nabla V\f$ on the surface of a unit-sphere which we refer to as _the manifold_ from here on.
Missiles do not generate a force field but move in \f$\vec\nabla V\f$ whereas planets are static and stay fix.
%Planets and missiles are placed on the manifold and the dynamics of the latter is constrained to not leave it at any time.
Let \f$(\vec q, \vec p) \in \mathbb{R}^3 \otimes \mathbb{R}^3\f$ be the phase space coordinates of a single missile and let us think about \f$\vec q\f$ as the spatial position which is sufficient to evaluate \f$V\f$, such that the Hamiltonian of a single missile with mass \f$m\f$ reads:
\f[
    H(\vec{q}, \vec{p}) = \frac{p^2}{2m} + V(\vec{q}) + \lambda g(\vec{q}) \,,
\f]
where \f$g(\vec q)\f$ is the constraint of the manifold
\f{align*}{
    g(\vec q) &= \frac{1}{2} \left( q^2 - 1 \right) = 0 \,, \\
    \vec\nabla g(\vec q) &= \vec{q} \,.
\f}
The potential \f$V(\vec q)\f$ is the linear superposition of the contributions of each planet \f$i\f$ at position \f$\vec{y}_i \in \mathbb{R}^3\f$,
\f[
    V(\vec q) = \sum\limits_{i=1}^n V_d(\sigma_i) \,,
\f]
with
\f[
    \sigma_i = \arccos(\vec{q} \cdot \vec{y}_i) \,,
\f]
where \f$\sigma_i\f$ is the (shortest) distance between \f$\vec q\f$ and \f$\vec{y}_i\f$ on the manifold.
On a sphere, the shortest distance is always an orthodrome, also known as a great circle.
Without loss of generality, the kinematics of a massive particle in the force field of a single planet can thus be reduced to the movement on a (static) unit-circle.
Further, let's assume that the particle is at the azimuth \f$\varphi = \sigma \in [0, 2\pi)\f$ and is attracted by the planet at \f$\varphi = 0\f$.
Following the line of sight of the particle on the manifold, the planet is *seen* periodically at distances \f$\sigma, 2\pi + \sigma, \ldots\f$ in one direction and at \f$2\pi - \sigma, 4\pi - \sigma, \ldots\f$ in the opposite direction.

We implement two different types of gravitation potentials \f$V_d(\sigma_i)\f$,
\f{align*}{
    V_{2\mathrm{D}}(\sigma_i) &= -2 \sum\limits_{j=0}^\infty V^{(j)}_{2\mathrm{D}}(\sigma_i) = -2 \sum\limits_{j=0}^\infty \big[ \ln(2\pi j + \sigma_i) + \ln(2\pi (j+1) - \sigma_i) \big] , \\
    V_{3\mathrm{D}}(\sigma_i) &= -\frac{1}{4\pi} \sum\limits_{j=0}^\infty V^{(j)}_{3\mathrm{D}}(\sigma_i) = -\frac{1}{4\pi} \sum\limits_{j=0}^\infty \left[ \frac{1}{2\pi j + \sigma_i} + \frac{1}{2\pi (j+1) - \sigma_i} \right] ,
\f}
where the former contributes \f$\sim \sigma_i^{-1}\f$ force fields that correspond to a 2D fields solely embedded on the manifold and the latter is motivated by the canonical \f$\sim \sigma_i^{-2}\f$ behavior of the unconstrained 3D case.

In the figure below we show \f$V^{(j)}_{3\mathrm{D}}(\sigma_i)\f$ for \f$j=0,1,2\f$.

[<img src="pot3D.png" width="400"/>](pot3D.png)

Note that these potential or only physical meaningfull for \f$0 \le \sigma_i < 2\pi\f$.
In fact, by definition \f$\sigma_i \in [0, \pi]\f$ since \f$\sigma_i = \pi\f$ is the largest possible distance between two points on a unit-sphere and thus any reasonable potential \f$V(\sigma_i)\f$ has to obey the symmetry relation \f$V(\sigma_i) = V(2\pi - \sigma_i)\f$.

The force fields are given by the gradients of the respective potentials:
\f[
    \vec\nabla_{\!q} V(\vec{q}) = \sum\limits_{i=1}^n \frac{V'_d(\sigma_i)}{\sin \sigma_i} \, \vec{y}_i = \sum\limits_{i=1}^n f_d(\sigma_i) \, \vec{y}_i
\f]
where we introduced the abbreviations
\f{align*}{
    V'_{2\mathrm{D}}(\sigma) &= -\sum\limits_{i=1}^n \operatorname{cot} \frac{\sigma}{2} \\
    V'_{3\mathrm{D}}(\sigma) &= -\sum\limits_{i=1}^n (\pi - \sigma) \sum\limits_{j=0}^\infty \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\pi - \sigma)^2 \right]^2}
\f}
and 
\f{align*}{
    f_{2\mathrm{D}}(\sigma_i) &= -\frac{1}{1 - \cos \sigma_i} \\
    f_{3\mathrm{D}}(\sigma_i) &= -\frac{1}{\operatorname{sinc}(\sigma_i - \pi)} \sum\limits_{j=0}^\infty \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\sigma_i - \pi)^2 \right]^2} \\
    &= \lim\limits_{J \to \infty} \sum\limits_{j=0}^J f^{(j)}_{3\mathrm{D}}(\sigma_i)
\f}
with \f$d \in \{2\mathrm{D}, 3\mathrm{D}\}\f$.
Below, we show both types of the scaling factors \f$f_d\f$ as functions of \f$\sigma_i\f$.
Note that we use different scales of the ordinate to compensate the effect of our arbitrarly chosen coupling constants.
Furthermore, note that \f$f_d\f$ is only a scaling factor of the force field.
In particular, \f$f_d(\sigma_i) < 0 \; \forall \, \sigma_i\f$ whereas the projection of \f$\vec\nabla_{\!q} V(\vec{q})\f$ onto the manifold can become zero, e.g., if \f$n=1\f$ and \f$\vec{q} \parallel \vec{y}_i\f$.

[<img src="fd.png" width="400"/>](fd.png)

In the visualization of \f$d = 3\mathrm{D}\f$ above we approximated the infinite sum with \f$J=100\f$ summands.
Below, we show the ratio of truncated summations w.r.t. \f$J=0\f$.
In general, we see that the sum converges fast and for most cases taking \f$\mathrm{O}(10)\f$ summands is sufficient. 

[<img src="f3D_ratio.png" width="400"/>](f3D_ratio.png)

\f{align*}{
    H^{[1]} &= \frac{p^2}{2m} + \lambda g(\vec{q}) \\
    H^{[2]} &= V(\vec{q}) + \lambda g(\vec{q})
\f}

\f[
    \begin{pmatrix}
        \vec{q} \\ \vec{p}
    \end{pmatrix}
    \overset{\phi_t^{[1]}}{\longleftarrow}
    \begin{pmatrix}
        \frac{1}{p} \, \vec{p} \, \sin p \tau + \vec{q} \, \cos p \tau \\
        -p \, \vec{q} \, \sin p \tau + \vec{p} \, \cos p \tau 
    \end{pmatrix}
\f]

\f[
    \tau = \frac{t}{m}
\f]

with approximation
\f{align*}{
    \cos x &= 1 - \frac{x^2}{2} + \mathcal{O}(x^4) \\
    \sin x &= \sqrt{1 - \cos^2(x)} = x \, \sqrt{1 - \frac{x^2}{4}} + \mathcal{O}(x^3)
\f}
\f[
    \begin{pmatrix}
        \vec{q} \\ \vec{p}
    \end{pmatrix}
    \overset{\phi_t^{[1]}}{\longleftarrow}
    \begin{pmatrix}
        \vec{q} + \tau \, \sqrt{1 - \frac{p^2 \tau^2}{4}} \, \vec{p} - \frac{p^2 \tau^2}{2} \, \vec{q} \\
        \vec{p} - p^2 \tau \, \sqrt{1 - \frac{p^2 \tau^2}{4}} \, \vec{q} - \frac{p^2 \tau^2}{2} \, \vec{p}
    \end{pmatrix}
\f]

\f[
    \begin{pmatrix}
        \vec{q} \\ \vec{p}
    \end{pmatrix}
    \overset{\phi_t^{[2]}}{\longleftarrow}
    \begin{pmatrix}
        \vec{q} \\
        \vec{p} + \left[ (\vec{q} \cdot \vec\nabla_{\!q} V(\vec{q})) \, \vec{q} - \vec\nabla_{\!q} V(\vec{q}) \right] t
    \end{pmatrix}
\f]

Strang splitting \f$\phi_{t/2}^{[1]} \circ \phi_t^{[2]} \circ \phi_{t/2}^{[1]}\f$

\f[
    \vec{x} = \begin{pmatrix}
        \cos\phi \, \sin\lambda \\
        \cos\phi \, \cos\lambda \\
        \sin\phi
    \end{pmatrix}
\f]
\f[
    \vec{v} = \begin{pmatrix}
        v_1 \\ v_2 \\ v_3
    \end{pmatrix} = \dot\phi \begin{pmatrix}
        -\sin\phi \, \sin\lambda \\
        -\sin\phi \, \cos\lambda \\
        \cos\phi
    \end{pmatrix} + \dot\lambda \, \cos \phi \begin{pmatrix}
        \phantom{-}\cos\lambda \\
        -\sin\lambda \\
        0
    \end{pmatrix}
\f]

Note that we refer in source to \f$\dot\phi\f$ and \f$(\dot\lambda \, \cos \phi)\f$ as `vlat` and `vlon`, respectively.

\f[
    \begin{pmatrix}
        \dot\lambda \, \cos \phi \\
        -\dot\phi \, \sin \phi
    \end{pmatrix} = \begin{pmatrix}
        \cos \lambda & -\sin \lambda \\
        \sin \lambda & \cos \lambda
    \end{pmatrix} \begin{pmatrix}
        v_1 \\ v_2
    \end{pmatrix}
\f]
if \f$|\sin \phi| \ll |\cos \phi|\f$
\f[
    \dot \phi = \frac{v_z}{\cos \phi}
\f]

\f$x^2=1\f$ and \f$v^2 = \dot\phi^2 + (\dot\lambda \, \cos \phi)^2\f$

Rotating \f$\vec{\mathrm{e}}_z = (0, 0, 1)^\top\f$ to latitude \f$\phi\f$ and longitude \f$\lambda\f$
\f[
    \boldsymbol{R}_{\phi, \lambda} = \begin{pmatrix}
        -\cos\lambda & -\sin\phi \, \sin\lambda & \cos\phi \, \sin\lambda \\
        \phantom{-}\sin\lambda & -\sin\phi \, \cos\lambda & \cos\phi \, \cos\lambda \\
        0 & \cos\phi & \sin\phi
    \end{pmatrix}
\f]

Velocity vector with distance \f$r\f$ to \f$(\phi, \lambda)\f$
\f[
    \vec v = v \, \boldsymbol{R}_{\phi, \lambda} \begin{pmatrix}
        \cos r \, \sin \psi \\
        \cos r \, \cos \psi \\
        -\sin r
    \end{pmatrix}
\f]
(using \f$\sin(\pi/2 - r) = \cos(r)\f$ and \f$\cos(\pi - r) = \sin(r)\f$)

starting from
\f[
    \vec x = \begin{pmatrix} x_1 \\ x_2 \\ x_3 \end{pmatrix} = \boldsymbol{R}_{\phi, \lambda} \begin{pmatrix}
        \sin r \, \sin \psi \\
        \sin r \, \cos \psi \\
        \cos r
    \end{pmatrix}
\f]

\f{align*}{
    \phi' &= \arcsin x_3 \\
    \lambda' &= \operatorname{atan2}(x_1, x_2)
\f}

\f[
    \vec{v} = \dot\phi \begin{pmatrix}
        -\sin\phi' \, \sin\lambda' \\
        -\sin\phi' \, \cos\lambda' \\
        \cos\phi'
    \end{pmatrix} + \dot\lambda \, \cos\phi' \begin{pmatrix}
        \phantom{-}\cos\lambda' \\
        -\sin\lambda' \\
        0
    \end{pmatrix}
\f]

\f[
    \dot\phi = \vec v \cdot \begin{pmatrix}
        -\sin\phi' \, \sin\lambda' \\
        -\sin\phi' \, \cos\lambda' \\
        \cos\phi'
    \end{pmatrix}
\f]
\f[
    \dot\lambda \, \cos\phi' = \vec v \cdot \begin{pmatrix}
        \phantom{-}\cos\lambda' \\
        -\sin\lambda' \\
        0
    \end{pmatrix}
\f]

## How to proceed from here on
You now have all the information necessary to efficiently deal with [our API](@ref include/api.h).
Go there to find out how to use it in practice for fun and profit.
