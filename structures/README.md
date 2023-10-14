# Martin Bartender

_NOTA: Todo está indexado en $$0$$._

#### Subtarea 1. _$$a_i=a_j$$ para cualquier par de $$i, j$$._

Martin siempre escogerá a los **K** más cercanos. 

No incluyendo el $$a_i$$; en la posición $$0$$, serían los primeros **K**. Dando un total de $$K(K-1)/2$$.

$$0+1+2+3... + K$$

En la posición $$1$$, serían los primeros **K**. Dando un total de 1 + $$(K-1)(K-2)/2$$.

$$1+0+1+2+...+(K-1)$$

En algún punto tendrías algo como esto. 
* Si **K** es impar
    * $$(K-1)/2 + (K-3)/2 ... + 1 + 0 + 1 + (K-3)/2 + (K-1)/2$$
* Si **K** es par
    *  $$(K/2 + (K-3)/2 ... + 1 + 0 + 1 + (K-2)/2$$

Para la posición $$i$$. Se buscará lo de arriba, que es lo más óptimo, determinaremos los límites y de ahí partimos. 
Si **K** es impar:
* m = $$min(i, min(N-i-1, (K-1)/2))$$

Si **K** es par:
* m = $$min(i, min(N-i-1, K/2))$$

**respuesta** = $$m*(m-1)/2$$ + $$(K-m) * (K-m-1)/2$$

La respuesta final sería **respuesta** + ($$a_0*K$$)

$$O(N)$$

#### Subtarea 2. _$$K=N$$_
Martin siempre escogerá a todos. 

No incluyendo el $$a_i$$; podemos utilizar la idea de la subtarea 1, si $$K=N$$, los **N** más cercanos también satisfacen las condiciones. 

Lo único que cambia es la respuesta final; que en este caso sería **respuesta** + $$Σa_i$$.

$$O(N)$$


#### Subtarea 3. _$$N<=1000$$_

Se realizará una bruta. Para cada posición $$j$$ agregamos a un vector todos los elementos con su costo $$a_i+|i-j|$$. Y ordenamos, o usamos una priority_queue que se ajuste a los costos menores. Seleccionamos los **K** menores. 


$$O(N^2log_2N)$$

#### Subtarea 4. $$K=1$$

Consideremos un arreglo de $$costos$$, donde $$costos_i=a_i + i$$.
 
 Si estuvieramos en la posición $$j$$ y tuviera la certeza que la mejor opción es $$i$$, donde $$i>=j$$. Podríamos usar el arreglo inicial de $$costos$$ y determinar que para $$respuesta_j$$ = $$costos_i$$ - j.
 
 Resolveremos esto a manera de barrido. Tendremos una priority_queue<pair<int, int>> {$$costo_i$$, $$i$$}. Evidentemente tendrá en el top al menor. 
 En la primera iteración (índice $$0$$) tranquilamente escogeremos el top y lo definiremos como MEJOR. Pero conforme vayamos avanzando tendremos varias opciones.
* Actualizamos MEJOR 
    * El MEJOR sigue estándo adelante. O sea $$indice>=i$$. Se le resta 1. 
    * El MEJOR pasa a estar detrás. O sea $$indice<i$$. Se le suma 1. 
 * Vemos el siguiente posible MEJOR
    * El índice del top es mayor que $$i$$: hacemos pop y actualizamos MEJOR=$$top-i$$ solo si esto disminuye el valor de MEJOR.
    * El índice del top es menor que $$i$$: Le hacemos pop y repetimos hasta que la primera condición sea verdadera.
 
¿Por qué hacemos pop si el $$índice$$ es menor que $$i$$?

Es fácil de ver que si ese top fuese una opción se hubiera escogido antes. Cuando el índice era mayor que alguna $$i$$.

#### Código
```cpp
estructura MEJOR = costos.top(); costos.pop();
    for(int i = 0; i < n; i++){
        cout<<MEJOR.v<<" ";
        if(MEJOR.i<=i) MEJOR.v++;
        else MEJOR.v--;
        while(costos.size() && costos.top().i<=i)
            costos.pop();
        if(costos.size() && costos.top().v-(i+1)<MEJOR.v){
            MEJOR.v = costos.top().v-(i+1);
            MEJOR.i = costos.top().i;
            costos.pop();
        }
    }
```

$$O(Nlog_2N)$$

#### Solución

Usamos la idea de la subtarea 4, pero esta vez hay que mantener **K MEJORES**. Evidentemente eliminaremos el peor de los **MEJORES**, que siempre estarán a la izquierda. Y agregamos de la derecha.  Esto se puede llevar también con una priority_queue. Tendremos que saber cuántos de nuestros MEJORES están a la derecha **H**, y cuántas a la izquierda **G**. Para restar **H** y sumar **G**.


