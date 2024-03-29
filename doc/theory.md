The propagation of point-like particles, aka _missiles_, are simulated in the gravitational force field of static center of masses, aka _planets_, on the surface of a unit sphere.

In this document, we describe in detail the theoretical background and list all equations that are necessary to efficiently deal with this library.
If you prefer a more experimental-orientated approach you can skip this section and directly jump to [the description of the API](@ref API).
There, you will find everything necessary to use the library and to run your simulations.

We also provide a minimalistic [game extension](@ref game) that is built on top of the API.
Read its documentation [here](@ref game) if you are interested in using it as a building block for your game server.

# Theory
Missiles do not generate a force field but move in \f$\vec\nabla V\f$ whereas planets are static and stay fixed.
Let \f$(\vec q, \vec p) \in \mathbb{R}^3 \otimes \mathbb{R}^3\f$ be the phase space coordinates of a single missile and let us think about \f$\vec q\f$ as the spatial position which is sufficient to evaluate \f$V\f$ such that the Hamiltonian of a single missile with mass \f$m\f$ reads:
\f[
    H(\vec{q}, \vec{p}) = \frac{p^2}{2m} + V(\vec{q}) + \lambda g(\vec{q}) \,,
\f]
where \f$g(\vec q)\f$ is the constraint of the manifold
\f{align*}{
    g(\vec q) &= \frac{1}{2} \left( q^2 - 1 \right) = 0 \,, \\
    \vec\nabla g(\vec q) &= \vec{q} \,.
\f}

**NB:** Differentiating the constraint \f$g(\vec q) = 0\f$ w.r.t. time we get the identity \f$\vec q \perp \vec p\f$, and differentiating a second time a physical interpretation of the Lagrange multiplier \f$\lambda = p^2 - \vec{q} \cdot \vec\nabla V\f$:
For particles at rest the projection of the gravitational force along the position vector of a given particle thus has to be compensated in order to fulfill the constraint of our manifold.
If the particle has a finite momentum, this constraining force can be reduced (note that \f$\operatorname{sign}(\vec{q} \cdot \vec\nabla V) > 0\f$) until the gravitational force pulls the particle on a circular orbit (\f$ \lambda = 0\f$) and eventually changes from a _pulling_ force into a _pushing_ force if the momentum becomes even larger.

The conservative potential \f$V(\vec q)\f$ is the linear superposition of the contributions of each planet \f$i\f$ at position \f$\vec{y}_i \in \mathbb{R}^3\f$,
\f[
    V(\vec q) = \sum\limits_{i=1}^n V_d(\sigma_i) \,,
\f]
with
\f[
    \sigma_i = \arccos(\vec{q} \cdot \vec{y}_i) \,,
\f]
where \f$0 \le \sigma_i \le \pi \f$ is the (shortest) distance between \f$\vec q\f$ and \f$\vec{y}_i\f$ on the manifold.
On a sphere, the shortest distance is an orthodrome, also known as a great circle.
Without loss of generality, the kinematics of a missile in the force field of a single planet can thus be reduced to the movement on a (static) unit circle.
Further, let's assume that the missile is at the azimuth \f$\varphi = \sigma \in [0, 2\pi)\f$ and is attracted by the planet at \f$\varphi = 0\f$.
Following the line of sight of the missile on the manifold, the planet is *seen* periodically at distances \f$\sigma, 2\pi + \sigma, \ldots\f$ in one direction and at \f$2\pi - \sigma, 4\pi - \sigma, \ldots\f$ in the opposite direction.

We implement two different types of gravitation potentials \f$V_d(\sigma_i)\f$,
\f{align*}{
    V_{2\mathrm{D}}(\sigma_i) = 2 \sum\limits_{j=0}^\infty V^{(j)}_{2\mathrm{D}}(\sigma_i) &= 2 \sum\limits_{j=0}^\infty \big[ \ln(2\pi j + \sigma_i) + \ln(2\pi (j+1) - \sigma_i) - 2 \ln \pi (2j+1) \big] \\
    &= 2 \ln \sin \frac{\sigma_i}{2}, \\
    V_{3\mathrm{D}}(\sigma_i) = \frac{1}{4\pi} \sum\limits_{j=0}^\infty V^{(j)}_{3\mathrm{D}}(\sigma_i) &= -\frac{1}{4\pi} \sum\limits_{j=0}^\infty \left[ \frac{1}{2\pi j + \sigma_i} + \frac{1}{2\pi (j+1) - \sigma_i} - \frac{2}{\pi (2j+1)} \right] \\
    &= -\frac{2 \psi\!\left( \frac{1}{2} \right) - \psi\!\left(\frac{\sigma_i}{2 \pi} \right) - \psi\!\left(1 - \frac{\sigma_i}{2 \pi} \right)}{8 \pi^2}
\f}
with the digamma function \f$\psi\f$.
The former contributes force fields \f$\sim \sigma_i^{-1}\f$ that correspond to a 2D field solely embedded on the manifold and the latter is motivated by the canonical \f$\sim \sigma_i^{-2}\f$ behavior of the unconstrained 3D case.
Both potentials are gauged by an additional constant offset s.t. \f$V_{2\mathrm{D}}(\pi) = V_{3\mathrm{D}}(\pi) = 0\f$ which keeps each series convergent while not inferring with the respective force fields.

In the figure below, we show \f$V^{(j)}_{3\mathrm{D}}(\sigma_i)\f$ for \f$j=0,1,2\f$.

[<img src="pot3D.png" width="400"/>](pot3D.png)

Note that these potentials are only physical meaningful for \f$0 \le \sigma_i < 2\pi\f$.
(In fact, by definition of the inverse cosine function, the interval actual reads \f$0 \le \sigma_i \le \pi\f$.)
Furthermore, since \f$\sigma_i = \pi\f$ is the largest possible distance between two points on a unit sphere, any reasonable potential \f$V(\sigma_i)\f$ has to obey the symmetry relation \f$V(\sigma_i) = V(2\pi - \sigma_i)\f$.

The aforementioned gauging of the potentials makes the evaluation of the escape velocity, \f$p_\pi\f$, at a given distance to a planet, \f$\delta\f$, straightforward:
\f[
    p_\pi = \sqrt{-2m \, V_d(\delta)} \,,
\f]
where we define \f$p_\pi\f$ as the minimal initial momentum necessary to asymptotically reach a distance \f$\sigma = \pi\f$ when launched at a distance \f$\delta\f$.
In layman's terms: <em>The only way to not shoot yourself in the face is to launch a missile faster than \f$p_\pi\f$—then you will hit your back.</em>
Obviously, these definitions only hold for a single planet.

**Tip:** In practical applications \f$p_\pi\f$ can be used as a natural unit for velocities and time.
For example, velocities can be given in multiples of \f$p_\pi\f$ and durations in multiples of the orbital period if missiles are launched with \f$2p_\pi\f$.
Use grvx_v_esc() and grvx_orb_period() to get \f$p_\pi\f$ and the orbital period, respectively, of the given potential.

The force fields are given by the gradients of the respective potentials:
\f[
    -\vec\nabla_{\!q} V(\vec{q}) = -\sum\limits_{i=1}^n \frac{V'_d(\sigma_i)}{\sin \sigma_i} \, \vec{y}_i = \sum\limits_{i=1}^n f_d(\sigma_i) \, \vec{y}_i
\f]
where we introduced the abbreviations
\f{align*}{
    V'_{2\mathrm{D}}(\sigma_i) &= \operatorname{cot} \frac{\sigma_i}{2} \\
    V'_{3\mathrm{D}}(\sigma_i) &= (\pi - \sigma_i) \sum\limits_{j=0}^\infty \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\pi - \sigma_i)^2 \right]^2}
\f}
and 
\f{align*}{
    f_{2\mathrm{D}}(\sigma_i) &= -\frac{1}{1 - \cos \sigma_i} \\
    f_{3\mathrm{D}}(\sigma_i) &= \lim\limits_{J \to \infty} \sum\limits_{j=0}^J f^{(j)}_{3\mathrm{D}}(\sigma_i) \\
    &= -\frac{1}{\operatorname{sinc}(\sigma_i - \pi)} \, \lim\limits_{J \to \infty} \sum\limits_{j=0}^J \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\sigma_i - \pi)^2 \right]^2}
\f}
with \f$d \in \{2\mathrm{D}, 3\mathrm{D}\}\f$ and \f$\operatorname{sinc} \bullet = (\sin \bullet) / \bullet\f$.
Below, we show both types of the scaling factors \f$f_d\f$ as functions of \f$\sigma_i\f$.
Note that we use different scales of the ordinate to compensate effects of our arbitrarly chosen coupling constants.
Furthermore, note that \f$f_d\f$ is a scaling factor and not the force field.
(For a single planet, \f$n=1\f$, however, it is equivalent to the magnitude of the force field.)
In particular, \f$f_d(\sigma_i) < 0 \; \forall \, \sigma_i\f$ whereas the projection of \f$\vec\nabla_{\!q} V(\vec{q})\f$ onto the manifold can become zero, e.g., if \f$n=1\f$ and \f$\vec{q} \parallel \vec{y}_i\f$.

[<img src="fd.png" width="400"/>](fd.png)

In the visualization of \f$d = 3\mathrm{D}\f$ above, we approximated the infinite sum with \f$J=100\f$ summands.
Below, we show the ratio of truncated summations w.r.t. \f$J=0\f$.
In general, we see that the sum converges fast and for most cases taking \f$\mathrm{O}(10)\f$ summands is sufficient. 
In particular, the deviation is largest where the force field is weakest and the missile kinematic is most likely be dominated by other planets, and even here, taking the first two leading terms, \f$J=1\f$, already gives an approximation that deviates less than \f$2\,\%\f$ from the asymptotic result, \f$J \to \infty\f$.

[<img src="f3D_ratio.png" width="400"/>](f3D_ratio.png)

## Just in case you care
For a given universe with a single planet, there are circular orbits which keep a constant distance \f$r = \sigma_1\f$ to the planet (aka [_small circles_](https://en.wikipedia.org/wiki/Circle_of_a_sphere)), as well as a constant velocity \f$v_\mathrm{orb}\f$.
By setting the planet at one of the poles, \f$y_1 = (0, 0, 1)^\top\f$, we obtain a simple ODE from the Hamiltonian:
\f[
    \begin{pmatrix} \ddot x_1 \\ \ddot x_2 \\ \ddot x_3 \end{pmatrix} =
    f_d \!\left( \arccos(x_3) \right) \begin{pmatrix} 0 \\ 0 \\ 1 \end{pmatrix}
    \mp \lambda \begin{pmatrix} x_1 \\ x_2 \\ x_3 \end{pmatrix},
\f]
where \f$r = \arccos x_3\f$ is the distance to the planet.
Here, a constant distance is equivalent to \f$\dot x_3 = \ddot x_3 = 0\f$ and thus:
\f[
    \ddot x_3 = 0 \qquad \Leftrightarrow \qquad \lambda = \frac{f_d \!\left( \arccos(x_3) \right)}{|x_3|} = v_\lambda^2 = \text{const.}
\f]
Hence, a possible solution reads:
\f[
    \begin{pmatrix} x_1 \\ x_2 \\ x_3 \end{pmatrix} =
    \begin{pmatrix}
        \cos\phi_0 \, \sin v_\lambda t \\
        \cos\phi_0 \, \cos v_\lambda t \\
        \sin\phi_0
    \end{pmatrix}.
\f]
The velocity \f$v_\mathrm{orb}\f$ for a given force field \f$f_d\f$ and distance \f$r\f$ can thus be obtained by setting \f$\phi_0 = \pi/2 - r\f$ and evaluating:
\f[
    v_{\mathrm{orb},d} \equiv \dot x = v_\lambda \cos \phi_0 = \sqrt{ \frac{-f_d(r)}{|\cos r|}} \, \sin r \,.
\f]
For convenience, we provide a helper function grvx_v_scrcl() which returns \f$v_\mathrm{orb}\f$ if provided with \f$r\f$.

Note, that for \f$d=2\mathrm{D}\f$ the relation simplifies significantly and allows for a straightforward evaluation of \f$v_{\mathrm{orb},2\mathrm{D}}\f$:
\f[
    v_{\mathrm{orb},2\mathrm{D}} = \sqrt{ \frac{1 + \cos r}{|\cos r|}}.
\f]

Below, we show \f$v_{\mathrm{orb},d}(r)\f$ for both types of potentials:
Naïvely, one might expect a similar \f$\sim 1/\sqrt{r}\f$ behavior that we know from our canonical \f$\sim 1/r\f$ potential on earth.
In curved space, however, free particles propagate on [great circles](https://en.wikipedia.org/wiki/Great_circle) and thus increasingly higher velocities are needed if the small circle converges towards a great circle at a distance of \f$r = \pi / 2\f$.
At the maximal distance \f$r = \pi\f$, the small circle converges into a (unstable) fix point.
For \f$r \to 0\f$, our 2D potential becomes \f$\sqrt{2}\f$ whereas \f$v_{\mathrm{orb},3\mathrm{D}}\f$ diverges.

[<img src="vscrcl.png" width="400"/>](vscrcl.png)

# Symplectic integration

We use a [Strang splitting](https://doi.org/10.1137/0705041) \f$\phi_t = \phi_{t/2}^{[1]} \circ \phi_t^{[2]} \circ \phi_{t/2}^{[1]}\f$ to integrate the Hamiltonian system \f$H = H^{[1]} + H^{[2]}\f$ with
\f{align*}{
    H^{[1]} &= \frac{p^2}{2m} + \lambda g(\vec{q}) \\
    H^{[2]} &= V(\vec{q}) + \lambda g(\vec{q}) \,.
\f}
Without loss of generality, we choose the scale of a time-step and the coupling constant of the potential such that we can set \f$m=1\f$ during simulation, i.e.,
\f{align*}
    \frac{t}{m} &\mapsto t , \\
    Vm &\mapsto V .
\f}
By using this convention, the integration steps read:
\f{align*}
    \begin{pmatrix}
        \vec q \\ \vec p
    \end{pmatrix}
    &\overset{\phi_t^{[1]}}{\longleftarrow}
    \begin{pmatrix}
        \frac{1}{p} \, \vec p  \, \sin pt + \vec q \, \cos pt \\
        -p \, \vec q \, \sin pt + \vec p \, \cos pt 
    \end{pmatrix} = \begin{pmatrix}
        \vec q \\ \vec p
    \end{pmatrix} + \begin{pmatrix} 
        \vec p \, t \operatorname{sinc} pt - 2 \vec q \sin^2 pt/2 \\
        -\vec q p^2 \, t \operatorname{sinc} pt - 2 \vec p \sin^2 pt/2
    \end{pmatrix} , \\
    \begin{pmatrix}
        \vec q \\ \vec p
    \end{pmatrix}
    &\overset{\phi_t^{[2]}}{\longleftarrow}
    \begin{pmatrix} \vec q \\ \vec p \end{pmatrix} + \begin{pmatrix} 0 \\ \vec a t \end{pmatrix} 
\f}
with \f$\vec a = (\vec q \cdot \vec\nabla_{\!q} V(\vec q)) \, \vec q - \vec\nabla_{\!q} V(\vec q)\f$ and \f$\operatorname{sinc}(\bullet) = (\sin \bullet) / \bullet\f$.
(\f$\phi_t^{[1]}\f$ propagates the missile on a great-circle as if no gravitational forces were present and \f$\phi_t^{[2]}\f$ corrects the momentum.)

Alternatively, the phase space can be parametrized with \f$(\hat q = \vec q, \hat p = \vec p / p, p)\f$ where \f$\hat\bullet\f$ are unit vectors and \f$p\f$ is the momentum magnitude:
\f{align*}
    \begin{pmatrix}
        \hat q \\ \hat p \\ p
    \end{pmatrix}
    &\overset{\phi_t^{[1]}}{\longleftarrow}
    \begin{pmatrix}
        +\hat q \, \cos pt + \hat p \, \sin pt \\
        -\hat q \, \sin pt + \hat p \, \cos pt \\
        p
    \end{pmatrix} = \begin{pmatrix}
        \hat q \\ \hat p \\ p
    \end{pmatrix} + \begin{pmatrix} 
        -2 \hat q \sin^2 pt/2 + \hat p \sin pt \\
        -2 \hat p \sin^2 pt/2 - \hat q \sin pt \\
        0
    \end{pmatrix} , \\
    \begin{pmatrix}
        \hat q \\ \hat p \\ p
    \end{pmatrix}
    &\overset{\phi_t^{[2]}}{\longleftarrow}
    \begin{pmatrix}
        \hat q \\
        \left| p \hat p + \vec a t \right|^{-1} \, \left( p \hat p + \vec a t \right) \\
        |p\hat p + \vec a t|
    \end{pmatrix} ,
\f}
Here, the update step for \f$\hat p\f$ can become error-prone if \f$\left| p \hat p + \vec a t \right| \ll 1\f$.
This is the case if either \f$p \hat p \approx -\vec a t\f$ but \f$p \gg 0\f$ or if \f$\mathcal{O}(p) = \mathcal{O}(|\vec a t |) \ll 1\f$.
In both cases, keeping the old value of \f$\hat p\f$ but setting \f$p = 0\f$ gives a reasonable approximation and pins the missile in the latter case onto a fix point.
However, we prefer the first formulation of \f$\phi_t^{[1]}\f$ and \f$\phi_t^{[2]}\f$ operating on \f$(\vec p, \vec q)\f$ since it lends itself for an efficient numerical implementation using [Higham's summation scheme](https://doi.org/10.1137/0914050).

The strang splitting \f$\phi_t = \phi_{t/2}^{[1]} \circ \phi_t^{[2]} \circ \phi_{t/2}^{[1]}\f$ is a second-order symmetric and symplectic integration scheme.
We offer a set of several symmetric composition schemes during compilation, each raising the integration order by evaluating \f$\phi_t\f$ in multiple stages and refer to them as `pXsY` where `X` and `Y` are the new integration order and the number of stages, respectively:

 - `p2s1`: Vanilla Strang splitting.
 - `p4s3`: Triple Jump. See, [DOI:10.1103/PhysRevLett.63.9](https://doi.org/10.1103/PhysRevLett.63.9) or related publications for more information.
 - `p4s5`: Suzuki's Fractal. See [DOI:10.1016/0375-9601(90)90962-N](https://doi.org/10.1016/0375-9601(90)90962-N) for more information.
 - `p6s9`: Kahan & Li (1997). See [DOI:10.1090/S0025-5718-97-00873-9](https://doi.org/10.1090/S0025-5718-97-00873-9) for more information.
 - `p8s15`: Suzuki & Umeno (1993). See [DOI:10.1007/978-3-642-78448-4_7](https://doi.org/10.1007/978-3-642-78448-4_7) for more information.

Since the Strang splitting is symplectic, its compositions are symplectic as well.

Asymptotically, increasing the integration scheme will eventually outperform smaller time-steps.
However, for practical applications and finite integration times, one should always carefully benchmark simulation speed and accuracy w.r.t. time-step size and integration order.

# Spherical coordinates

[Our API](@ref API) is designed asymmetrically w.r.t. coordinate systems. 
Internally, our algorithms operate on Cartesian coordinates whereas the initialization of planets and missiles has to be given in spherical coordinates.
Doing so takes the burden off the caller to correctly embed vectors onto the manifold and makes the physical interpretation of parameters easier.

A point on the surface of a unit sphere has two degrees of freedom which we refer to as latitude, \f$\phi\f$, and longitude, \f$\lambda\f$.
In Cartesian coordinates such a point is given by
\f[
    \vec{x} = \begin{pmatrix}
        \cos\phi \, \sin\lambda \\
        \cos\phi \, \cos\lambda \\
        \sin\phi
    \end{pmatrix} .
\f]

Similarly, a velocity vector is given by
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
    \end{pmatrix} ,
\f]
and is always orthogonal to \f$\vec x\f$ at the same latitude and longitude.
We define \f$\dot\phi\f$ and \f$\dot\lambda \, \cos\phi\f$ as the latitudinal velocity, grvx_vlat(), and the (scaled) longitudinal velocity, grvx_vlon(), respectively.
Adding both in quadrature yields the magnitude of the Cartesian velocity,
\f[
     v = \sqrt{\dot\phi^2 + (\dot\lambda \, \cos \phi)^2} \,.
 \f]

Typically, missiles are launched at the rim of a planet, where _rim_ refers to a small circle centered at the given planet.
The velocity vector of a missile launched at a distance \f$\sigma\f$ to \f$(\phi, \lambda)\f$ is given by
\f[
    \vec v = v \, \boldsymbol{R}_{\phi, \lambda} \begin{pmatrix}
        \cos \sigma \, \sin \psi \\
        \cos \sigma \, \cos \psi \\
        -\sin \sigma
    \end{pmatrix},
\f]
where the rotating matrix \f$\boldsymbol{R}_{\phi, \lambda}\f$ rotates \f$\vec{\mathrm{e}}_z = (0, 0, 1)^\top\f$ to latitude \f$\phi\f$ and longitude \f$\lambda\f$,
\f[
    \boldsymbol{R}_{\phi, \lambda} = \begin{pmatrix}
        -\cos\lambda & -\sin\phi \, \sin\lambda & \cos\phi \, \sin\lambda \\
        \phantom{-}\sin\lambda & -\sin\phi \, \cos\lambda & \cos\phi \, \cos\lambda \\
        0 & \cos\phi & \sin\phi
    \end{pmatrix}
\f]
and \f$\psi\f$ is the initial longitude prior to the rotation.
This velocity vector is located at a point with latitude \f$\phi' \neq \phi\f$ and longitude \f$\lambda' \neq \lambda\f$,
\f{align*}{
    \phi' &= \arcsin x_3 \,, \\
    \lambda' &= \operatorname{atan2}(x_1, x_2) \,,
\f}
where \f$(x_1, x_2, x_3)^\top\f$ is given by
\f[
    \vec x = \begin{pmatrix} x_1 \\ x_2 \\ x_3 \end{pmatrix} = \boldsymbol{R}_{\phi, \lambda} \begin{pmatrix}
        \sin \sigma \, \sin \psi \\
        \sin \sigma \, \cos \psi \\
        \cos \sigma
    \end{pmatrix}.
\f]
Hence, an equivalent parametrization of \f$\vec v\f$ is
\f[
    \vec{v} = \dot\phi \begin{pmatrix}
        -\sin\phi' \, \sin\lambda' \\
        -\sin\phi' \, \cos\lambda' \\
        \cos\phi'
    \end{pmatrix} + \dot\lambda \, \cos\phi' \begin{pmatrix}
        \phantom{-}\cos\lambda' \\
        -\sin\lambda' \\
        0
    \end{pmatrix}.
\f]
that allows for a straightforward extraction of the longitudinal and (scaled) latitudinal velocities:
\f[
    \dot\phi = \vec v \cdot \begin{pmatrix}
        -\sin\phi' \, \sin\lambda' \\
        -\sin\phi' \, \cos\lambda' \\
        \cos\phi'
    \end{pmatrix},
\f]
\f[
    \dot\lambda \, \cos\phi' = \vec v \cdot \begin{pmatrix}
        \phantom{-}\cos\lambda' \\
        -\sin\lambda' \\
        0
    \end{pmatrix}.
\f]

# Angular resolution
Let's assume that an observation \f$\vec{x}_\mathrm{obs}\f$ on a planet at \f$(\phi_\mathrm{p}, \lambda_\mathrm{p})\f$ of an event happening somewhere in the universe is measured with a finite angular resolution \f$\delta \alpha\f$.
Hence, the exact position \f$\alpha\f$ on a [small circle](https://en.wikipedia.org/wiki/Circle_of_a_sphere) with a radius \f$\sigma\f$ around the planet is unknown:
\f[
    \vec{x}_\mathrm{obs} = \boldsymbol{R}_{\phi_\mathrm{p}, \lambda_\mathrm{p}}
    \begin{pmatrix}
        \sin \sigma \, \sin (\alpha + \delta \alpha) \\
        \sin \sigma \, \cos (\alpha + \delta \alpha) \\
        \cos \sigma
    \end{pmatrix},
\f]
with \f$\sigma = \arccos( \vec{x}_\mathrm{obs} \cdot \vec{x}_\mathrm{p})\f$.
Consequently, the absolute error of the perturbed measurement \f$\vec{x}_\mathrm{obs} = \vec{x}_\mathrm{obs}(\alpha + \delta \alpha)\f$ and the ground truth event \f$\vec{x}_\mathrm{gt} = \vec{x}_\mathrm{obs}(\alpha + 0)\f$,
\f[
    \left| \vec{x}_\mathrm{obs} - \vec{x}_\mathrm{gt} \right| = 2 \sin \left( \frac{\delta \alpha}{2} \right) \, \sin \sigma,
\f]
scales linearly with \f$\sin \sigma = \sqrt{ 1 - ( \vec{x}_\mathrm{obs} \cdot \vec{x}_\mathrm{p} )^2}\f$.
We implement this kind of perturbation and expose it via grvx_perturb_measurement().

**NB:** The mapping between the position on a small circle \f$\alpha\f$ and the measurement \f$(\phi_\mathrm{obs}, \lambda_\mathrm{obs})\f$ is given by \f$\alpha = \operatorname{atan2}(y_1, y_2)\f$ with
\f{align*}
    y_1 &= \cos \phi_\mathrm{obs} \, \sin(\lambda_\mathrm{p} - \lambda_\mathrm{obs}) \\
    y_2 &= \cos \phi_\mathrm{p} \, \sin \phi_\mathrm{obs} - \sin \phi_\mathrm{p} \, \cos \phi_\mathrm{obs} \, \cos(\lambda_\mathrm{p} - \lambda_\mathrm{obs})
\f}
where we have defined \f$\vec{y} = (y_1, y_2, y_3)^\top\f$ as
\f[
    \vec y \equiv \boldsymbol{R}^{-1}_{\phi_\mathrm{p}, \lambda_\mathrm{p}} \vec{x}_\mathrm{obs} = \boldsymbol{R}^\top_{\phi_\mathrm{p}, \lambda_\mathrm{p}} \vec{x}_\mathrm{obs} \,.
\f]

# Using the adjoint method to infer planet configurations
**TL;DR:** We don't expose an implementation of the adjoint method (yet).

The adjoint method is an effective tool to find the gradient of a scalar loss \f$F\f$ w.r.t. a set of parameters of a differential equation. (Have a look into Bradley's tutorial about the adjoint method, ["PDE-constrained optimization and the adjoint method" (2010)](https://cs.stanford.edu/~ambrad/adjoint_tutorial.pdf), to learn more!)
In our case, \f$F\f$ could be the integrated error between the actual and a predicted trajectory, where the latter is the result of an _exact_ simulation based on spurious planet positions \f$\vec{y}_i\f$.
\f[
    F(\vec q) = \int\limits_0^T \!\mathrm{d}t \, f(\vec q, t) = \int\limits_0^T \!\mathrm{d}t \, \sum\limits_{k=1}^M \delta(t-t_k) \, \varepsilon_k(\vec q(t)),
\f]
where \f$\varepsilon_k\f$ with \f$k = 1, \ldots, M\f$ is the residual of the \f$k\f$-th measurement at time \f$t = t_k\f$.

The adjoint method gives a recipe to find the trajectory of a new variable \f$\vec a \in \mathbb{R}^6\f$, referred to as "the adjoint", that can be used to find the exact gradient
\f[
    \vec\nabla_{y_i}F = \int\limits_T^0 \!\mathrm{d}t \, \left( \frac{\partial \vec h}{\partial \vec{y}_i} \right)^\top \vec{a}(t) \,,
\f]
where \f$\vec h\f$ summarizes the dynamics of our Hamiltonian system:
\f[
    \vec h(\vec q, \vec p, \vec y_1, \ldots, \vec y_n) = \begin{pmatrix}
        \vec p \\
        -\vec\nabla_{\!q} V - \vec q \left( p^2 - \vec q \cdot \vec\nabla_{\!q} V \right)
    \end{pmatrix}.
\f]

The gradient \f$\vec\nabla_{y_i}F\f$ can be used in a optimization framework, such as gradient descent, to infer the _genuine_ planet configuration starting only with an initial guess.
Clearly, the same method can be used to solve similar problems, e.g., finding optimal initial conditions for a missile's launch to hit a certain target.

The adjoint \f$\vec{a}(t)\f$ is itself the solution of an ODE:
\f[
    \dot{\vec{a}}(t) = -\left( \frac{\partial \vec h}{\partial (\vec q, \vec p)} \right)^{\!\top} \vec a(t) + \left( \frac{\partial f}{\partial (\vec q, \vec p)} \right)^{\!\top}
\f]
with the initial condition \f$a(T) = 0\f$.

Note that solving this new ODE and the integral used for obtaining the gradient requires the adjoint to be solved backwards in time.
Luckily, our solver for finding \f$(\vec q, \vec p)\f$ is symmetric such that we can avoid a memory intense caching (aka _checkpointing_) by simply simulating a missile with inverted momentum!

The three Jacobians needed for finding the gradients are
\f{align*}
    \left( \frac{\partial f}{\partial (\vec q, \vec p)} \right)^{\!\top} &=
    \sum_{k=1}^M \begin{pmatrix}
        \delta(t-t_k) \, \vec\nabla_{\!q} \varepsilon_k(\vec q(t)) \\
        \boldsymbol{0}
    \end{pmatrix}, \\
    \left( \frac{\partial \vec h}{\partial (\vec q, \vec p)} \right)^{\!\top} &=
    \begin{pmatrix}
        \boldsymbol{0} &
        -\vec\nabla_{\!q} \otimes \vec\nabla_{\!q} V - \boldsymbol{1} \left( p^2 - \vec q \cdot \vec\nabla_{\!q} V \right) + \vec q \otimes \vec\nabla V + \left( \vec q \otimes \vec q \right) \left( \vec\nabla_{\!q} \otimes \vec\nabla_{\!q} V \right) \\
        \boldsymbol{1} &
        -2 \, \vec q \otimes \vec p
    \end{pmatrix} \\
    &= \begin{pmatrix}
        \boldsymbol{0} &
        -\boldsymbol{1} p^2 \\
        \boldsymbol{1} &
        -2 \, \vec q \otimes \vec p
    \end{pmatrix} - \sum_{i=1}^n \begin{pmatrix}
        \boldsymbol{0} &
        f_d(\sigma_i) \cos \sigma_i  + \vec{\rho}_{i,d} \otimes \vec{y}_i \\
        \boldsymbol{0} & \boldsymbol{0}
    \end{pmatrix}, \\
    \left( \frac{\partial \vec h}{\partial \vec{y}_i} \right)^{\!\top} &=
    \begin{pmatrix}
        \boldsymbol{0} \\
        -\vec\nabla_{\!q} \otimes \vec\nabla_{\!y_i} V + \left( \vec q \otimes \vec q \right) \left( \vec\nabla_{\!q} \otimes \vec\nabla_{\!y_i} V \right)
    \end{pmatrix}^{\!\top} \\
    &= \begin{pmatrix}
        \boldsymbol{0} \\
        f_d(\sigma_i) - \vec{\rho}_{i,d} \otimes \vec q
    \end{pmatrix}^{\!\top},
\f}
where we introduced
\f[
    \vec{\rho}_{i,d} = f_d(\sigma_i) \, \vec q + f'_d(\sigma_i) \, \underbrace{\frac{\vec{y}_i - \vec q \cos \sigma_i}{\sin \sigma_i}}_{\vec{q}_{\!\perp}}
\f]
and already inserted the results of the Jacobians
\f{align*}
    \frac{\partial}{\partial \vec q} \left( \vec\nabla_{\!q} V \right)
    &\equiv \vec\nabla_{\!q} \otimes \vec\nabla_{\!q} V
    = \sum_{i=1}^n \frac{f'_d(\sigma_i)}{\sin \sigma_i} \, \vec{y}_i \otimes \vec{y}_i \,, \\
    \frac{\partial}{\partial \vec{y}_i} \left( \vec\nabla_{\!q} V \right)
    &\equiv \vec\nabla_{\!q} \otimes \vec\nabla_{\!y_i} V
    = -f_d(\sigma_i) + \frac{f'_d(\sigma_i)}{\sin \sigma_i} \vec{y}_i \otimes \vec q \,.
\f}

Note that \f$\vec{q}_{\!\perp} \perp \vec q\f$, \f$\vec{q}_{\!\perp} \perp (\vec q \times \vec{y}_i)\f$ and \f$q_{\!\perp} = 1\f$.
Numerically stable solutions of \f$\vec{q}_{\!\perp} = \vec{q}_{\!\perp}^{(j)}\f$ are given by \f$j = \operatorname{argmax} \vec q\f$ and
\f{align*}
    \vec{q}_{\!\perp}^{(1)} &\sim \begin{pmatrix}
        q_2 c_2 + q_3 c_3 \\
        -q_1 c_2 \\
        -q_1 c_3
    \end{pmatrix} &
    \vec{q}_{\!\perp}^{(2)} &\sim \begin{pmatrix}
        -q_2 c_1 \\
        q_1 c_1 + q_3 c_3 \\
        -q_2 c_3
    \end{pmatrix} &
    \vec{q}_{\!\perp}^{(3)} &\sim \begin{pmatrix}
        -q_3 c_1 \\
        -q_3 c_2 \\
        q_1 c_1 + q_2 c_2 \\
    \end{pmatrix} &
\f}
with \f$ \vec c = \vec q \times (\vec q \times \vec y_i)\f$.

We conjecture that the newly introduced ODEs should be symmetrized, e.g., using the (implicit) trapezoidal rule to stabilize the evaluation for long trajectories.
Although being implicit in general, the ODE for the gradient 
\f[
    \vec\nabla_{y_i} F = \int\limits_T^0 \!\mathrm{d}t \, \left( f_d(\sigma_i) - \vec q \otimes \vec{\rho}_{i,d} \right) \vec{a}_p
\f]
becomes explicit by using the [Sherman-Morrison formula](https://doi.org/10.1137%2F1031049).
However, the same does not hold for the adjoint ODE thus driving the application of the adjoint method numerically costly, in particular for \f$d=3\mathrm{D}\f$ where the derivative \f$f'_{3D}\f$ has to be found as well.

In this light, approximating the gradient in a finite-difference (FD) scheme appears to be favorable if the number of parameters, \f$3\times n\f$, is small.
We therefore refrain from implementing the adjoint method for the time being and motivate practioners to implement a parallelized FD scheme on top of our API.
Besides the fact that a similar parallelization of the adjoint method is not straight-forward to implement, an FD scheme also allows the direct extraction of the gradients w.r.t. two spherical coordinates per planet instead of three Cartesian coordinates.

# Detailed description of API
You now have all the information necessary to efficiently deal with [our API](@ref API).
Go there to find out how to use it in practice for fun and profit.
