\f[
    H(\vec{q}, \vec{p}) = \frac{p^2}{2m} + V(\vec{q}) + \lambda g(\vec{q})
\f]

\f{align*}{
    g(\vec{q}) &= \frac{1}{2} \left( q^2 - 1 \right) = 0 \\
    \vec\nabla g(\vec{q}) &= \vec{q}
\f}

\f{align*}{
    V_{2\mathrm{D}}(\sigma) &= -2 \sum\limits_{j=0}^\infty \big[ \ln(2\pi j + \sigma) + \ln(2\pi (j+1) - \sigma) \big] \\
    V_{3\mathrm{D}}(\sigma) &= -\frac{1}{4\pi} \sum\limits_{j=0}^\infty \left[ \frac{1}{2\pi j + \sigma} + \frac{1}{2\pi (j+1) - \sigma} \right]
\f}

\f{align*}{
    V'_{2\mathrm{D}}(\sigma) &= -\sum\limits_{i=1}^n \operatorname{cot} \frac{\sigma}{2} \\
    V'_{3\mathrm{D}}(\sigma) &= -\sum\limits_{i=1}^n (\pi - \sigma) \sum\limits_{j=0}^\infty \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\pi - \sigma)^2 \right]^2}
\f}

\f[
    V(\vec{q}) = \sum\limits_{i=1}^n V_d(\sigma_i)
\f]
with
\f[
    \sigma_i = \arccos(\vec{q} \cdot \vec{y}_i)
\f]

\f[
    \vec\nabla_{\!q} V(\vec{q}) = \sum\limits_{i=1}^n \frac{V'_d(\sigma_i)}{\sin \sigma_i} \, \vec{y}_i = \sum\limits_{i=1}^n \mathcal{V}_d(\sigma_i) \, \vec{y}_i
\f]

\f{align*}{
    \mathcal{V}_{2\mathrm{D}}(\sigma_i) &= -\frac{1}{1 - \cos \sigma_i} \\
    \mathcal{V}_{3\mathrm{D}}(\sigma_i) &= -\frac{1}{\operatorname{sinc}(\sigma_i - \pi)} \sum\limits_{j=0}^\infty \frac{2j + 1}{\left[ (2j+1)^2 \pi^2 - (\sigma_i - \pi)^2 \right]^2}
\f}

note that although \f$V'_{2\mathrm{D}}(\sigma_i) / \sin \sigma_i \neq 0\f$ at \f$\sigma_i = \pi\f$, here \f$\vec{q} \parallel \vec\nabla_{\!q} V\f$

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
