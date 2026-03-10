Udregnet i matlab:
$A = \sqrt{L_{p}^{2}+ \frac {L_{r}^{2}\sin(\theta_{2})^{2}} {4}}$
$$\begin{align*}
(J_{1}+ m_{2}A) \ddot{\theta}_{1}+ \frac {L_{r}^{2}m_{2}\cos(\theta_{2})\sin(\theta_{2})\dot{\theta}_{1} \dot{\theta}_{2}}  {4 A} &= \tau\\
J_{2} \ddot{\theta}_{2} + L_{r}gm_{2}\sin(\theta_{2})- \frac {L_{r}m_{2}\cos(\theta_{2})\sin(\theta_{2}) \dot{\theta}_{1}^{2}} {8 A} &= 0
\end{align*}$$
her lineariserer vi så omkring:
$$\begin{align*}
\cos(\theta_{2})&=  -1\\
\sin(\theta_{2})&= -\theta\\
\dot{\theta}_{1}^{2}&= \dot{\theta_{1}}
\end{align*}$$
her får vi så:
$$\begin{align*}
(J_{1}+ m_{2}A) \ddot{\theta}_{1}+ \frac {L_{r}^{2}m_{2}\theta_{2}\dot{\theta}_{1} \dot{\theta}_{2}}  {4 A} &= \tau\\
J_{2} \ddot{\theta}_{2} - L_{r}gm_{2}\theta_{2}- \frac {L_{r}m_{2}\theta_{2} \dot{\theta}_{1}} {8 A} &= 0
\end{align*}$$
$A = \sqrt{L_{p}^{2}+ \frac {L_{r}^{2}\theta_{2}} {4}}$
