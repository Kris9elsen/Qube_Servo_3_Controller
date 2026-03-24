![[Pasted image 20260315103332.png|565]]
![[Pasted image 20260317111941.png|505]]

## E_kin,trans
$$v_{2org}= L_{r} \cdot \begin{pmatrix}-\cos(\theta_{1}) \dot{\theta}_{1} \\ -\sin(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix}$$
$$\begin{align*}
v_{p}&=  v_{2org} +\frac{L_{p}}{2} \begin{pmatrix}- \sin(\theta_{1})\cos(\theta_{2}+90)  \dot{\theta}_{1} -  \cos(\theta_{1})\sin(\theta_{2}+90) \dot{\theta}_{2}
 \\ 
 \cos(\theta_{1}) \cos(\theta_{2}+90) \dot{\theta}_{1} - \sin(\theta_{1})\sin(\theta_{2}+90) \dot{\theta}_{2}
  \\ 
  - \cos(\theta_{2}+90)\dot{\theta}_{2}
\end{pmatrix}
\end{align*}$$
her har vi så:
$$\begin{align*}
\cos(\theta_{2}+ 90) &= \sin(-\theta_{2}) = -\sin(\theta_{2})\\
\sin(\theta_{2}+ 90 ) &= \cos(-\theta_{2}) = \cos(\theta_{2})
\end{align*}$$
$$\begin{align*}
v_{p}&=  v_{2org} +\frac{L_{p}}{2} \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix}
\end{align*}$$
For at finde den kinetiske energi har vi:
$$E_{kin}= \frac{1}{2}mv^{2}$$
vi har derfor brug for hastighederne kvadreret
$$\begin{align*}
|v_{2org}|^{2} &=  L_{r}^{2}\cdot \left (\sin^{2}(\theta_{1})\dot{\theta}_{1}^{2}+\cos^{2}(\theta_{1})\dot{\theta}_{1}^{2} \right )\\
|v_{2org}|^{2} &=  L_{r}^{2}\cdot \dot{\theta}_{1}^{2}\left (\sin^{2}(\theta_{1})+\cos^{2}(\theta_{1}) \right )\\
|v_{2org}|^{2} &=  L_{r}^{2}\cdot \dot{\theta}_{1}^{2}
\end{align*}$$
(Det giver god mening og der står sikkert noget lignende på et eller andet af Christophers modellering slides)
Vi havde før at:
$$v_{p}=  v_{2org} +\frac{L_{p}}{2} \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix}$$
som kan omskrives til:
$$v_{p}=   L_{r} \cdot \begin{pmatrix}-\cos(\theta_{1}) \dot{\theta}_{1} \\ -\sin(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix} +\frac{L_{p}}{2} \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix}$$
dette omskrives så til:
$$v_{p}= \frac{L_{p}}{2} \cdot \left (\frac{2L_{r}}{L_{p}} \cdot  \begin{pmatrix}-\cos(\theta_{1}) \dot{\theta}_{1} \\ -\sin(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix} + \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix} \right )$$
Eftersom vi er interesseret i længden af denne vektor for at finde den kinetiske energi og har $|k \cdot v| = |k||v|$ hvis k er en scalar, kan vi kigge skrive det som:
$$|v_{p}|= \left|\frac{L_{p}}{2} \right |_{}^{} \cdot|A|$$
$$A^{2} = \left| \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}-B \cos(\theta_{1})\dot{\theta}_{1}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}-B \sin(\theta_{1})\dot{\theta}_{1}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix} \right |_{}^{2}$$
hvor $B = \frac {2L_{r}} {L_{p}}$
$$A^{2} = 
\begin{align*}
&\sin^{2}(\theta_{1})\sin^{2}(\theta_{2})\dot{\theta}_{1}^{2}+\cos^{2}(\theta_{1})\cos^{2}(\theta_{2})\dot{\theta}_{2}^{2}-2 \dot{\theta}_{1}\dot{\theta}_{2} \sin(\theta_{1})\sin(\theta_{2})\cos(\theta_{1})\cos(\theta_{2}) 
\\
&+\cos^{2}(\theta_{1})\sin^{2}(\theta_{2}) \dot{\theta}_{1}^{2} +\sin^{2}(\theta_{1})\cos^{2}(\theta_{2})\dot{\theta}_{2}^{2} + 2\dot{\theta}_{1}\dot{\theta}_{2}\cos(\theta_{1})\sin(\theta_{2})\sin(\theta_{1})\cos(\theta_{2})\\
&+\sin^{2}(\theta_{2})\dot{\theta}_{2}^{2}
\\
&+B^{2}\cos^{2}(\theta_{1}) \dot{\theta}_{1}^{2}-2B\dot{\theta}_{1}^{2} \sin(\theta_{1}) \sin(\theta_{2})\cos(\theta_{1}) +2B \dot{\theta}_{1} \dot{\theta}_{2}  \cos^{2}(\theta_{1}) \cos(\theta_{2})
\\
&+B^{2}\sin^{2}(\theta_{1})\dot{\theta}_{1}^{2}+2B \dot{\theta}_{1}^{2} \cos(\theta_{1})\sin(\theta_{2}) \sin(\theta_{1})+2B \dot{\theta}_{1} \dot{\theta}_{2} \sin^{2}(\theta_{1})\cos(\theta_{2})
\end{align*}
$$
(De 3 øverste er når B = 0, og de resterende blev tilføjet bagefter og indeholder det tredje led kvadreret samt ganget med tilhørende)
faktorisere så i $\dot{\theta}_{1}^{2}$, $\dot{\theta}_{2}^{2}$, $2\dot{\theta}_{1}\dot{\theta}_{2}$, $B^{2} \dot{\theta}_{1}^{2}$, $2B \dot{\theta}_{1}^{2}$ og $2B \dot{\theta}_{1} \dot{\theta}_{2}$ 
$$A^{2} = 
\begin{align*}
&\dot{\theta}_{1}^{2}\left (
\sin^{2}(\theta_{1})\sin^{2}(\theta_{2})+\cos^{2}(\theta_{1})\sin^{2}(\theta_{2})
 \right )
\\
+&\dot{\theta}_{2}^{2}\left (
\cos^{2}(\theta_{1})\cos^{2}(\theta_{2})+\sin^{2}(\theta_{1})\cos^{2}(\theta_{2})+\sin^{2}(\theta_{2})
 \right )
\\
+&2\dot{\theta}_{1}\dot{\theta_{2}}\left (
\sin(\theta_{1})\sin(\theta_{2})\cos(\theta_{1})\cos(\theta_{2}) -\cos(\theta_{1})\sin(\theta_{2})\sin(\theta_{1})\cos(\theta_{2})
 \right )
 \\
+&B^{2} \dot{\theta}_{1}^{2}\left ( 
\sin^{2}(\theta_{1}) + \cos^{2}(\theta_{1})
\right )
\\
+&2B \dot{\theta}_{1}^{2} \left ( 
-\sin(\theta_{1})\sin(\theta_{2})\cos(\theta_{1})+\cos(\theta_{1})\sin(\theta_{2})\sin(\theta_{1})
\right )
\\
+&2B \dot{\theta}_{1} \dot{\theta}_{2}\left ( 
\cos^{2}(\theta_{1})\cos(\theta_{2})+\sin^{2}(\theta_{1})\cos(\theta_{2})
\right )
\end{align*}$$
Nu handler det så bare om at faktorisere så meget som muligt og lede efter $\sin^{2}(\theta)+ \cos^{2}(\theta) = 1$
$$A^{2} = 
\begin{align*}
&\dot{\theta}_{1}^{2}\left (
\left(\sin^{2}(\theta_{1})+\cos^{2}(\theta_{1})\right)\cdot\sin^{2}(\theta_{2})
 \right )
\\
+&\dot{\theta}_{2}^{2}\left (
\left(\cos^{2}(\theta_{1})+\sin^{2}(\theta_{1})\right)\cdot\cos^{2}(\theta_{2})+\sin^{2}(\theta_{2})
 \right )
\\
+&B^{2}\dot{\theta}_{1}^{2}
\\
+&2B \dot{\theta}_{1}\dot{\theta}_{2}\cos(\theta_{2})
\end{align*}$$
Her var det trejde og femte led 0

$$\begin{align*}
A^{2} &= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}\left (\cos^{2}(\theta_{2})+\sin^{2}(\theta_{2})\right )+B^{2}\dot{\theta}_{1}^{2}
+2B \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2})\\
A^{2}&= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+B^{2}\dot{\theta}_{1}^{2}
+2B \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2})\\
\end{align*}
$$
eftersom havde:
$$|v_{p}|= \left|\frac{L_{p}}{2} \right |_{}^{} \cdot|A|$$
og der derfor gælder:
$$v_{p}^{2}=\frac{L_{p}^{2}}{4} \cdot A^{2}$$
har vi nu hastigheden af massemidtpunktet for pendulet.
vi kan nu skrive den kinetiske translatoriske energi op:
$$E_{kin,tran} = \frac{1}{2} \left (m_{p}v_{p}^{2}+m_{r}v_{2org}^{2} \right )$$
hvor:
$$\begin{align*}
v_{p}^{2}&= \frac{L_{p}^{2}}{4} \cdot A^{2}\\
A^{2}&= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+B^{2}\dot{\theta}_{1}^{2}
+2B \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2})\\
B &= \frac {2L_{r}} {L_{p}}\\
B^{2}&= \frac {4L_{r}^{2}} {L_{p}^{2}}\\
v_{2org}^{2} &=  L_{r}^{2}\cdot \dot{\theta}_{1}^{2}
\end{align*}$$
## E_kin,rot 
Vi skal her finde den rotationele energ
$$E_{kin,rot} = \frac{1}{2} \left (J_{p} \dot{\theta}_{2}^{2}+J_{1} \cdot \dot{\theta}_{1}^{2} \right )$$
Her har vi dog at $J_{1}$ er rundt om frame 1 z aksen så vi har:
$$J_{1} = J_{r}+m_{p}\cdot \left |{}^{B}p_{p,xy}\right|^{2}$$
$$|{}^{B}p_{p,xy}|^{2} = \left|\begin{pmatrix}-L_{r}\sin(\theta_{1})+ \frac{L_{p}}{2} \cos(\theta_{1})\cos(\theta_{2}+ 90) \\ L_{r}\cos(\theta_{1})+ \frac{L_{p}}{2} \sin(\theta_{1})\cos(\theta_{2}+90)\end{pmatrix}\right|^{2}$$
Her har vi igen $\cos(\theta_{2}+90 ) -\sin(\theta_{2})$
$$|{}^{B}p_{p,xy}|^{2} = \left|\begin{pmatrix}-L_{r}\sin(\theta_{1})- \frac{L_{p}}{2} \cos(\theta_{1})\sin(\theta_{2}) \\ L_{r}\cos(\theta_{1})- \frac{L_{p}}{2} \sin(\theta_{1})\sin(\theta_{2})\end{pmatrix}\right|^{2}$$
nu kan vi så regne det
$$|{}^{B}p_{p,xy}|^{2} =\begin{align*}
  &L_{r}^{2}\sin^{2}(\theta_{1})+ \frac {L_{p}^{2}} {4}\cos^{2}(\theta_{1})\sin^{2}(\theta_{2})+L_{r} L_{p}\cos(\theta_{1})\sin(\theta_{2})\sin(\theta_{1})\\
  +& L_{r}^{2}\cos^{2}(\theta_{1})+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{1})\sin^{2}(\theta_{2})-L_{r}L_{p}\sin(\theta_{1})\sin(\theta_{2})\cos(\theta_{1})
\end{align*}$$
Her faktorisere vi så $L_{r}^{2}$, $\frac {L_{p}^{2}} {4}$ og $L_{r}L_{p}$
$$|{}^{B}p_{p,xy}|^{2} = \begin{align*}
&L_{r}^{2}\left (\cos^{2}(\theta_{1})+\sin^{2}(\theta_{1}) \right )\\
+& \frac {L_{p}^{2}} {4} \left (\cos^{2}(\theta_{1})\sin^{2}(\theta_{2})+\sin^{2}(\theta_{1})\sin^{2}(\theta_{2}) \right )\\
+& L_{r}L_{p}\left (\cos(\theta_{1})\sin(\theta_{2})\sin(\theta_{1})-\sin(\theta_{1})\sin(\theta_{2})\cos(\theta_{1} \right )
\end{align*}$$
Her kan vi så reducerer en del:
$$|{}^{B}p_{p,xy}|^{2} = L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2})$$
Det kan vi så indsætte
$$J_{1} = J_{r}+m_{p}\cdot \left ( L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right )$$

## E_kin
nu har vi så den samlede kinetiske energi:
$$E_{kin}= E_{kin, trans} + E_{kin, rot}$$
hvor:
$$\begin{align*}
E_{kin,tran} &=  \frac{1}{2} \left (m_{p}v_{p}^{2}+m_{r}v_{2org}^{2} \right )\\
v_{p}^{2}&= \frac{L_{p}^{2}}{4} \cdot A^{2}\\
A^{2}&= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+B^{2}\dot{\theta}_{1}^{2}
+2B \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2})\\
B &= \frac {2L_{r}} {L_{p}}\\
B^{2}&= \frac {4L_{r}^{2}} {L_{p}^{2}}\\
v_{2org}^{2} &=  L_{r}^{2}\cdot \dot{\theta}_{1}^{2}
\\\\
E_{kin,rot} &=  \frac{1}{2} \left (J_{p} \dot{\theta}_{2}^{2}+J_{1} \cdot \dot{\theta}_{1}^{2} \right )\\
J_{1} &=  J_{r}+m_{p}\cdot \left ( L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right )

\end{align*}$$


## E_pot
Den potentielle energi kan findes som tyngdekraften i pendulet
$$E_{pot} = \frac{L_{p}}{2} \cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))$$
Nu kan vi skrive lagrangen op:

## lagrange
$$\begin{align*}
L &=  E_{kin}- E_{pot}\\
L &=  E_{kin, trans} + E_{kin, rot} - E_{pot}\\
L &=  \frac{1}{2} \left (m_{p}v_{p}^{2}+m_{r}v_{2org}^{2} \right ) + \frac{1}{2} \left (J_{p} \dot{\theta}_{2}^{2}+J_{1} \cdot \dot{\theta}_{1}^{2} \right ) - \frac{L_{p}}{2} \cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))
\\
L &=  \frac{1}{2} \left (m_{p}\frac{L_{p}^{2}}{4} \cdot A^{2}+m_{r}L_{r}^{2}\cdot \dot{\theta}_{1}^{2} \right ) + \frac{1}{2} \left (J_{p} \dot{\theta}_{2}^{2}+ \left ( J_{r}+m_{p}\cdot \left ( L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right ) \right ) \cdot \dot{\theta}_{1}^{2} \right )\\ 
&- \frac{L_{p}}{2} \cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))
\\
L &=  \frac{1}{2} \left (m_{p}\frac{L_{p}^{2}}{4} \cdot \left (\dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+\frac {4L_{r}^{2}} {L_{p}^{2}}\dot{\theta}_{1}^{2}
+2 \frac {2L_{r}} {L_{p}} \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2}) \right )+m_{r}L_{r}^{2}\cdot \dot{\theta}_{1}^{2} \right )\\ 
&+ \frac{1}{2} \left (J_{p} \dot{\theta}_{2}^{2}+ \left ( J_{r}+m_{p}\cdot \left ( L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right ) \right ) \cdot \dot{\theta}_{1}^{2} \right )\\ 
&- \frac{L_{p}}{2} \cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))\\
2L &=   m_{p}\frac{L_{p}^{2}}{4} \cdot \left (\dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+\frac {4L_{r}^{2}} {L_{p}^{2}}\dot{\theta}_{1}^{2}
+2 \frac {2L_{r}} {L_{p}} \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2}) \right )+m_{r}L_{r}^{2}\cdot \dot{\theta}_{1}^{2} \\ 
&+J_{p} \dot{\theta}_{2}^{2}+ \left ( J_{r}+m_{p}\cdot \left ( L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right ) \right ) \cdot \dot{\theta}_{1}^{2} - L_{p}\cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))
\\


\end{align*}$$
$$\begin{align*}
2L &=   m_{p}\frac{L_{p}^{2}}{4} \cdot \left (\dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+\frac {4L_{r}^{2}} {L_{p}^{2}}\dot{\theta}_{1}^{2}
+2 \frac {2L_{r}} {L_{p}} \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2}) \right )+m_{r}L_{r}^{2}\cdot \dot{\theta}_{1}^{2} \\ 
&+J_{p} \dot{\theta}_{2}^{2}+  J_{r}\dot{\theta}_{1}^{2}+m_{p}\cdot \left ( L_{r}^{2}\dot{\theta}_{1}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \dot{\theta}_{1}^{2}\right ) - L_{p}\cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))
\\
2L &=   m_{p}\frac{L_{p}^{2}}{4} \cdot \left (\dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+
 \right )+ L_{r}L_{p}m_{p} \dot{\theta}_{1}\dot{\theta}_{2} \cos(\theta_{2})+L_{r}^{2}m_{p}\dot{\theta}_{1}^{2}+m_{r}L_{r}^{2}\cdot \dot{\theta}_{1}^{2} \\ 
&+J_{p} \dot{\theta}_{2}^{2}+  J_{r}\dot{\theta}_{1}^{2}+m_{p}\cdot \left ( L_{r}^{2}\dot{\theta}_{1}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \dot{\theta}_{1}^{2}\right ) - L_{p}\cdot g \cdot m_{p} \cdot (1- \cos(\theta_{2}))\\
\end{align*}$$
Gider ikke reducerer mere så nu finder vi laver vi euler lagrange

## Euler lagrange
$$\frac {d} {dt} \frac {\partial L} {\partial \dot{x}}- \frac {\partial L} {\partial x} = Q$$
$$\begin{align*}
x = \begin{pmatrix}\theta_{1}\\
\theta_{2}\end{pmatrix}
\end{align*}$$

$$\begin{align*}
\frac {\partial L} {\partial x} &= \frac{1}{2}\cdot \begin{pmatrix}
0 \\
m_{p} \frac {L_{p}^{2}} {2} \dot{\theta}_{1}^{2} \sin(\theta_{2})\cos(\theta_{2})-L_{r}L_{p}m_{p}\dot{\theta}_{1}\dot{\theta}_{2}\sin(\theta_{2})+m_{p} \frac {L_{p}^{2}}{2} \dot{\theta}_{1}^{2}  \cos(\theta_{2})\sin(\theta_{2})-L_{p}gm_{p}\sin(\theta_{2})
\end{pmatrix}\\
\frac {\partial L} {\partial x} &= \frac{1}{2}\cdot \begin{pmatrix}
0 \\
m_{p} L_{p}^{2} \dot{\theta}_{1}^{2} \sin(\theta_{2})\cos(\theta_{2})-L_{r}L_{p}m_{p}\dot{\theta}_{1}\dot{\theta}_{2}\sin(\theta_{2})-L_{p}gm_{p}\sin(\theta_{2})
\end{pmatrix}
\end{align*}$$

$$\begin{align*}
\frac {\partial L} {\partial \dot{x}} &=  \frac{1}{2}\begin{pmatrix}
m_{p} \frac {L_{p}^{2}} {2}\sin^{2}(\theta_{2})\dot{\theta}_{1}+L_{r}L_{p}m_{p}\dot{\theta}_{2}\cos(\theta_{2})+2L_{r}^{2}m_{p}\dot{\theta}_{1}+2m_{r}L_{r}^{2}\dot{\theta}_{1}+2J_{r}\dot{\theta_{1}}+2m_{p}L_{r}^{2}\dot{\theta}_{1}+ m_{p}\frac {L_{p}^{2}} {2}\sin^{2}(\theta_{2})\dot{\theta}_{1} 
\\
m_{p} \frac {L_{p}^{2}} {2}\dot{\theta}_{2}+L_{r}L_{p}m_{p}\dot{\theta}_{1}\cos(\theta_{2})+2J_{p}\dot{\theta}_{2} 
\end{pmatrix}\\
\frac {\partial L} {\partial \dot{x}} &=  \frac{1}{2}\begin{pmatrix}
m_{p} L_{p}^{2} \sin^{2}(\theta_{2})\dot{\theta}_{1}+L_{r}L_{p}m_{p}\dot{\theta}_{2}\cos(\theta_{2})+4L_{r}^{2}m_{p}\dot{\theta}_{1}+2m_{r}L_{r}^{2}\dot{\theta}_{1}+2J_{r}\dot{\theta}_{1}
\\
m_{p} \frac {L_{p}^{2}} {2}\dot{\theta}_{2}+L_{r}L_{p}m_{p}\dot{\theta}_{1}\cos(\theta_{2})+2J_{p}\dot{\theta}_{2} 
\end{pmatrix}
\end{align*}$$
$$\frac {d} {dt} \frac {\partial L} {\partial \dot{x}} = \frac{1}{2}\begin{pmatrix}
m_{p}L_{p}^{2} \left (\sin^{2}(\theta_{2})\ddot{\theta}_{1}+2\cos(\theta_{2})\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right ) +L_{r}L_{p}m_{p} \left (\ddot{\theta}_{2}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}^{2} \right )+4L_{r}^{2}m_{p}\ddot{\theta}_{1}+2m_{r}L_{r}^{2}\ddot{\theta}_{1}+2J_{r}\ddot{\theta}_{1} 
\\
m_{p} \frac {L_{p}^{2}} {2}\ddot{\theta}_{2}+L_{r}L_{p}m_{p}\left (\ddot{\theta}_{1}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right )+2J_{p}\ddot{\theta_{2}}
\end{pmatrix} $$
$$Q = \begin{pmatrix}\tau_{m} \\ 0\end{pmatrix}$$

nu kan vi skrive det op:
$$
\frac{1}{2}\begin{pmatrix}
m_{p}L_{p}^{2} \left (\sin^{2}(\theta_{2})\ddot{\theta}_{1}+2\cos(\theta_{2})\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right ) +L_{r}L_{p}m_{p} \left (\ddot{\theta}_{2}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}^{2} \right )+4L_{r}^{2}m_{p}\ddot{\theta}_{1}+2m_{r}L_{r}^{2}\ddot{\theta}_{1}+2J_{r}\ddot{\theta}_{1} 
\\
m_{p} \frac {L_{p}^{2}} {2}\ddot{\theta}_{2}+L_{r}L_{p}m_{p}\left (\ddot{\theta}_{1}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right )+2J_{p}\ddot{\theta_{2}}\end{pmatrix}
- \frac{1}{2}\cdot \begin{pmatrix}
0 \\
m_{p} L_{p}^{2} \dot{\theta}_{1}^{2} \sin(\theta_{2})\cos(\theta_{2})-L_{r}L_{p}m_{p}\dot{\theta}_{1}\dot{\theta}_{2}\sin(\theta_{2})-L_{p}gm_{p}\sin(\theta_{2})
\end{pmatrix}
 = \begin{pmatrix}\tau_{m}  \\ 0\end{pmatrix}$$
 Nu trækker vi dem så fra hinanden
 $$\frac{1}{2}\begin{pmatrix}
m_{p}L_{p}^{2} \left (\sin^{2}(\theta_{2})\ddot{\theta}_{1}+2\cos(\theta_{2})\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right ) +L_{r}L_{p}m_{p} \left (\ddot{\theta}_{2}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}^{2} \right )+4L_{r}^{2}m_{p}\ddot{\theta}_{1}+2m_{r}L_{r}^{2}\ddot{\theta}_{1}+2J_{r}\ddot{\theta}_{1} 
\\
m_{p} \frac {L_{p}^{2}} {2}\ddot{\theta}_{2}+L_{r}L_{p}m_{p}\left (\ddot{\theta}_{1}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right )+2J_{p}\ddot{\theta_{2}} - \left (m_{p} L_{p}^{2} \dot{\theta}_{1}^{2} \sin(\theta_{2})\cos(\theta_{2})-L_{r}L_{p}m_{p}\dot{\theta}_{1}\dot{\theta}_{2}\sin(\theta_{2})-L_{p}gm_{p}\sin(\theta_{2}) \right )\end{pmatrix}
 = \begin{pmatrix}\tau_{m}  \\ 0\end{pmatrix}$$
 Nu handler det så bare om at reducere
 Vi starter her med at kigge på den øverste ligning
 $$m_{p}L_{p}^{2} \left (\sin^{2}(\theta_{2})\ddot{\theta}_{1}+2\cos(\theta_{2})\sin(\theta_{2})\dot{\theta}_{2}\dot{\theta}_{1} \right ) +L_{r}L_{p}m_{p} \left (\ddot{\theta}_{2}\cos(\theta_{2})-\sin(\theta_{2})\dot{\theta}_{2}^{2} \right )+4L_{r}^{2}m_{p}\ddot{\theta}_{1}+2m_{r}L_{r}^{2}\ddot{\theta}_{1}+2J_{r}\ddot{\theta}_{1}= \tau_{m}$$


## Fixet inertimoment
Problemmet med det tidligere var at der var troet at man bare kan finde interimomentet ud fra massemidtpunket ganget med afstanden i anden, men det passer ikke. I stedet skal man finde inertimomentet rundt om en given akse igennem masse midpunktet og så bruge steiners sætning
![[Pasted image 20260317161544.png]]
$$J_{p,z} = L_{p}^{2} m_{p}\sin^{2}(\theta_{2})\cdot \frac{1}{12}$$
Her kan vi så bruge steiners sætning:
$$J_{z} = J_{C}+Ml^{2}$$
her er l afstanden som den skal forskydes med og det er jo så afstanden for masse midtpunktet ind til centrum af motoren og er fundet tidligere som:
$$|{}^{B}p_{p,xy}|^{2} = L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2})$$
 vi kan så indsætte det sammen og så får vi:
 $$\begin{align*}
J_{1}-J_{r}&= L_{p}^{2} m_{p}\sin^{2}(\theta_{2})\cdot \frac{1}{12}+ m_{p} \left (L_{r}^{2}+ \frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \right )\\
J_{1}-J_{r}&= L_{p}^{2} m_{p}\sin^{2}(\theta_{2})\cdot \frac{1}{12}+ m_{p}L_{r}^{2}+ m_{p}\frac {L_{p}^{2}} {4}\sin^{2}(\theta_{2}) \\
J_{1}-J_{r}&= L_{p}^{2} m_{p} \frac{1}{4}\left(\sin^{2}(\theta_{2})\cdot \frac{1}{3}+ \sin^{2}(\theta_{2})\right) + m_{p}L_{r}^{2}\\
J_{1}-J_{r}&= L_{p}^{2} m_{p} \frac{1}{4}\sin^{2}(\theta_{2})\cdot \frac{4}{3}+ m_{p}L_{r}^{2}\\
J_{1}-J_{r}&= L_{p}^{2} m_{p} \frac{1}{3}\sin^{2}(\theta_{2})+ m_{p}L_{r}^{2}\\
\end{align*}$$
Det her er så det rigtige inertimoment omkring frame 1 z akse for pendulet.



## Matlab svar
Har giver op på at udlede det selv og finde fejlen, men matlab har fået det til:

$$
\begin{pmatrix}
J_{r} \ddot{\theta}_{1} +\frac{7 {L_{p}}^2  m_{p} \ddot{\theta}_{1} }{12}+2 {L_{r}}^2  m_{p} \ddot{\theta}_{1} +{L_{r}}^2  m_{r} \ddot{\theta}_{1} -\frac{7 {L_{p}}^2  m_{p} {\cos (\theta_2 (t))}^2  \ddot{\theta}_{1} }{12}-\frac{L_{p} L_{r} m_{p} \sin (\theta_2 (t)) {{(\dot{\theta}_{2})}}^2 }{2}+\frac{L_{p} L_{r} m_{p} \cos (\theta_2 (t)) \ddot{\theta}_{2} }{2}+\frac{7 {L_{p}}^2  m_{p} \cos (\theta_2 (t)) \sin (\theta_2 (t)) \dot{\theta}_{2} \dot{\theta}_{1}}{6}=\tau \\
\frac{m_{p} {L_{p}}^2  \ddot{\theta}_{2} }{4}+\frac{L_{r} m_{p} \cos (\theta_2 (t)) L_{p} \ddot{\theta}_{1} }{2}+J_{p} \ddot{\theta}_{2} -\frac{7 m_{p} \cos (\theta_2 (t)) \sin (\theta_2 (t)) {L_{p}}^2  {{(\dot{\theta}_{1})}}^2 }{12}+\frac{g m_{p} \sin (\theta_2 (t)) L_{p}}{2}=0
\end{pmatrix}
$$
Nu handler det så om at isolerer $\ddot{\theta}_{1}$ og $\ddot{\theta}_{2}$
$$  \ddot{\theta}_{1} =  \frac{2\,\left(-7\,\mathrm{Lr}\,\sin\left(\theta _{2}\left(t\right)\right)\,{\mathrm{Lp}}^3\,{\mathrm{mp}}^2\,{\cos\left(\theta _{2}\left(t\right)\right)}^2\,{\left(\frac{\partial }{\partial t} \theta _{1}\left(t\right)\right)}^2+3\,\mathrm{Lr}\,\sin\left(\theta _{2}\left(t\right)\right)\,{\mathrm{Lp}}^3\,{\mathrm{mp}}^2\,{\left(\frac{\partial }{\partial t} \theta _{2}\left(t\right)\right)}^2+12\,\mathrm{Jp}\,\mathrm{Lr}\,\sin\left(\theta _{2}\left(t\right)\right)\,\mathrm{Lp}\,\mathrm{mp}\,{\left(\frac{\partial }{\partial t} \theta _{2}\left(t\right)\right)}^2-7\,\sin\left(\theta _{2}\left(t\right)\right)\,{\mathrm{Lp}}^4\,{\mathrm{mp}}^2\,\cos\left(\theta _{2}\left(t\right)\right)\,\frac{\partial }{\partial t} \theta _{2}\left(t\right)\,\frac{\partial }{\partial t} \theta _{1}\left(t\right)-28\,\mathrm{Jp}\,\sin\left(\theta _{2}\left(t\right)\right)\,{\mathrm{Lp}}^2\,\mathrm{mp}\,\cos\left(\theta _{2}\left(t\right)\right)\,\frac{\partial }{\partial t} \theta _{2}\left(t\right)\,\frac{\partial }{\partial t} \theta _{1}\left(t\right)+6\,\mathrm{Lr}\,g\,\sin\left(\theta _{2}\left(t\right)\right)\,{\mathrm{Lp}}^2\,{\mathrm{mp}}^2\,\cos\left(\theta _{2}\left(t\right)\right)+6\,\tau \,{\mathrm{Lp}}^2\,\mathrm{mp}+24\,\mathrm{Jp}\,\tau \right)}{7\,{\mathrm{Lp}}^4\,{\mathrm{mp}}^2+48\,\mathrm{Jp}\,\mathrm{Jr}-7\,{\mathrm{Lp}}^4\,{\mathrm{mp}}^2\,{\cos\left(\theta _{2}\left(t\right)\right)}^2+24\,{\mathrm{Lp}}^2\,{\mathrm{Lr}}^2\,{\mathrm{mp}}^2+28\,\mathrm{Jp}\,{\mathrm{Lp}}^2\,\mathrm{mp}+96\,\mathrm{Jp}\,{\mathrm{Lr}}^2\,\mathrm{mp}+12\,\mathrm{Jr}\,{\mathrm{Lp}}^2\,\mathrm{mp}+48\,\mathrm{Jp}\,{\mathrm{Lr}}^2\,\mathrm{mr}-12\,{\mathrm{Lp}}^2\,{\mathrm{Lr}}^2\,{\mathrm{mp}}^2\,{\cos\left(\theta _{2}\left(t\right)\right)}^2-28\,\mathrm{Jp}\,{\mathrm{Lp}}^2\,\mathrm{mp}\,{\cos\left(\theta _{2}\left(t\right)\right)}^2+12\,{\mathrm{Lp}}^2\,{\mathrm{Lr}}^2\,\mathrm{mp}\,\mathrm{mr}}
$$


