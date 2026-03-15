![[Pasted image 20260315103332.png|565]]
![[Pasted image 20260315103343.png]]
$$v_{2org}= L_{r} \cdot \begin{pmatrix}-\sin(\theta_{1}) \dot{\theta}_{1} \\ \cos(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix}$$
$$\begin{align*}
v_{p}&=  v_{2org} + \begin{pmatrix}- \frac {L_{p} \sin(\theta_{1})\cos(\theta_{2}+90)  \dot{\theta}_{1}} {2} - \frac {L_{p} \cos(\theta_{1})\sin(\theta_{2}+90) \dot{\theta}_{2}} {2}
 \\ 
 \frac {L_{p}\cos(\theta_{1}) \cos(\theta_{2}+90) \dot{\theta}_{1}} {2} - \frac {L_{p}\sin(\theta_{1})\sin(\theta_{2}+90) \dot{\theta}_{2}} {2}
  \\ 
  - \frac {L_{p}\cos(\theta_{2}+90)\dot{\theta}_{2}} {2}
\end{pmatrix}\\


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
$$v_{p}=   L_{r} \cdot \begin{pmatrix}-\sin(\theta_{1}) \dot{\theta}_{1} \\ \cos(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix} +\frac{L_{p}}{2} \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix}$$
dette omskrives så til:
$$v_{p}= \frac{L_{p}}{2} \cdot \left (\frac{2L_{r}}{L_{p}} \cdot  \begin{pmatrix}-\sin(\theta_{1}) \dot{\theta}_{1} \\ \cos(\theta_{1}) \dot{\theta}_{1} \\ 0\end{pmatrix} + \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}
  \\ 
   \sin(\theta_{2})\dot{\theta}_{2}
\end{pmatrix} \right )$$
Eftersom vi er interesseret i længden af denne vektor for at finde den kinetiske energi og har $|k \cdot v| = |k||v|$ hvis k er en scalar, kan vi kigge skrive det som:
$$|v_{p}|= \left|\frac{L_{p}}{2} \right |_{}^{} \cdot|A|$$
$$A^{2} = \left| \begin{pmatrix} \sin(\theta_{1})\sin(\theta_{2})  \dot{\theta}_{1} -  \cos(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}-B \sin(\theta_{1})\dot{\theta}_{1}
 \\ 
 -\cos(\theta_{1}) \sin(\theta_{2}) \dot{\theta}_{1} - \sin(\theta_{1})\cos(\theta_{2}) \dot{\theta}_{2}+B \cos(\theta_{1})\dot{\theta}_{1}
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
&+B^{2}\sin^{2}(\theta_{1}) \dot{\theta}_{1}^{2}-2B\dot{\theta}_{1}^{2} \sin^{2}(\theta_{1}) \sin(\theta_{2}) +2B \dot{\theta}_{1} \dot{\theta}_{2} \sin(\theta_{1}) \cos(\theta_{1}) \cos(\theta_{2})
\\
&+B^{2}\cos^{2}(\theta_{1})\dot{\theta}_{1}^{2}-2B \dot{\theta}_{1}^{2} \cos^{2}(\theta_{1})\sin(\theta_{2})-2B \dot{\theta}_{1} \dot{\theta}_{2} \cos(\theta_{1}) \sin(\theta_{1})\cos(\theta_{2})
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
-\sin^{2}(\theta_{1}) \sin(\theta_{2})- \cos^{2}(\theta_{1})\sin(\theta_{2})
\right )
\\
+&2B \dot{\theta}_{1} \dot{\theta}_{2}\left ( 
\sin(\theta_{1}) \cos(\theta_{1}) \cos(\theta_{2})-\cos(\theta_{1}) \sin(\theta_{1})\cos(\theta_{2})
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
-&2B \dot{\theta}_{1}^{2} \left ( \left (\sin^{2}(\theta_{1}) + \cos^{2}(\theta_{1}) \right ) \cdot \sin(\theta_{2}) \right )
\end{align*}$$
Her  var det sidste og tredje led lig 0

$$\begin{align*}
A^{2} &= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}\left (\cos^{2}(\theta_{2})+\sin^{2}(\theta_{2})\right )+B^{2}\dot{\theta}_{1}^{2}
-2B \dot{\theta}_{1}^{2} \sin(\theta_{2})\\
A^{2}&= \dot{\theta}_{1}^{2}
\sin^{2}(\theta_{2})+\dot{\theta}_{2}^{2}+B^{2}\dot{\theta}_{1}^{2}
-2B \dot{\theta}_{1}^{2} \sin(\theta_{2})
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
-2B \dot{\theta}_{1}^{2} \sin(\theta_{2})\\
B &= \frac {2L_{r}} {L_{p}}\\
v_{2org}^{2} &=  L_{r}^{2}\cdot \dot{\theta}_{1}^{2}
\end{align*}$$
