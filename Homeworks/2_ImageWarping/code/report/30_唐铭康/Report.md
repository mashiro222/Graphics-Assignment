---
typora-copy-images-to: report_img
---

# 2_ImageWarping

- ID: 30, 唐铭康, PB17010350
- 2020/3/1

## Abstract

编写图像处理的Qt GUI应用程序, 完成基本的交互功能, 使用IDW和RBF算法实现图像的Warping处理, 并通过三角形坐标差值填补白缝.

## Method

### Class Diagram

![image-20200301041157519](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301041157519.png)

![image-20200301043247289](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301043247289.png)

**简单说明**:

-  **ImageWarping**类即为MainWindow类, 包含窗口所有人机交互内容. 
- **ImageWidget**为图像窗口类, 存储当前的图像信息以及图像处理的具体实现函数.
-  **Warping**类包含WarpingIDW和WarpingRBF两个子类.
  - 两个子类**继承**Warping类的控制点录入处理方法(即Initpoints()函数)和根据已知Warping函数$f(p),\,p\in \Z^2$情形下最图像进行最终处理的操作(即Warp()函数);
  - 两个子类在于具体的Warping函数的计算(即warp_func()函数)上形成**多态**.

### UI

**菜单栏与工具栏的功能简介**

![image-20200301042023482](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301042023482.png)

- 工具栏第一行左边五个图标同属于**File工具栏**, 分别为**打开**(Open), **保存**(Save), **关闭当前图像**(Close), **撤销上一步图像处理**(Undo), **回到最开始的图像**(Restore).
- File工具栏右边的图标属于**Edit工具栏**, 依次包括**反色**(Inverse), **左右镜像**(Mirror_H), **上下镜像**(Mirror_V), **灰度图**(Greyscale); **开始设置Warping控制点**(SetWarp), **清空控制点**(ClearWarp), **使用IDW方法进行Warping**(IDW), **使用RBF方法进行Warping**(RBF), **显示或隐藏控制点**(HidePoints/ShowPoints).
- 工具栏第二行为**Settings工具栏**, 依次为**设置插值网格的大小**(Grid size), **设置是否将控制点一起进行Warp处理的勾选框**(Warp input points), **设置图像显示大小的滚动条**(Image Size).
- 菜单栏分为File和Edit两个, 分别对应File和Edit工具栏的全部内容, 其中File菜单栏中含有**另存为**(Save As)功能.
- 所有图标都由本人手绘完成; 若要正确显示图标, 要求将Resources文件夹放在该应用程序同一目录下.

![image-20200301042848431](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301042848431.png)

**图像窗口交互简介**

- **设置控制点**: 图像窗口处于**Setting状态**时, 在屏幕上通过点击和拖动可以完成控制点. 起点由绿色叉表示, 路径为绿色线段, 终点为蓝色叉. 无拖动的点击被视为不动点, 由蓝叉表示. 所有点不限制在图像内部.

  ![image-20200301043649102](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301043649102.png)

**部分功能详细说明**

- **Restore**: Restore的操作会被视作图像处理操作, 从而可以通过Undo撤销.

- **SetWarp**: 点击后, 若当前窗口有图像成功打开, 则进入**Setting状态**.

- **ClearWarp**: 点击后, 当前所有控制点会被移除, 窗口离开Setting状态(进入**None状态**).

- **IDW/RBF**: 点击后, 若窗口处于Settting状态, 则相应进入**IDW/RBF状态**. 图像处理完毕后, 窗口返回**Setting状态**.

- **Hide/ShowPoints**: 点击后, 窗口切换控制点显示/不显示. 默认状态下, 所有控制点都会被显示, 该按钮为闭上的眼睛; 若控制点被隐藏, 按钮显示为睁开的眼睛, 可以点击再次显示控制点.

- **Grid size**: 本程序使用三角形网格的**Gouraud shading**着色策略, 每个三角形由用户设置大小的网格生成, 即从矩形对角线一分为二得到. 这里的数值即为矩形网格边长, 单位为像素. 该数值默认由图像的大小计算得到, 即$size=1+min/20$, 这里$min$表示图像较短边的长度. 没有图像时, 显示为-1.

- **Warp input points**: 默认不选中. 当该选项框被选中时, 控制点在Warping操作后也会跟着变化, 对应到Warping后的图像位置. 效果如下(上图为原图和控制点, 下图为一次RBF Warping后的图像和控制点).

  ![image-20200301045527480](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301045527480.png)

  ![image-20200301045536042](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301045536042.png)

- **Image Size**: 默认为100%, 范围为1% - 300%.

### Warping

#### IDW

$$
\pmb{f}(\pmb{p})=\sum w_i(\pmb{p})\pmb{f}_i(\pmb{p})
$$

其中

$$
w_i(\pmb{p})=\frac{\sigma_i(\pmb{p})}{\sum \sigma_j(\pmb{p})}
$$

$$
\sigma_i(\pmb{p})=\frac{1}{\|\pmb{p}-\pmb{p_i}\|^u}
$$

显然 $0\le w_i(\pmb{p})\le 1$，且 $\sum w_i(\pmb{p})=1$ .

又由基函数表达式
$$
\pmb{f}_i(\pmb{p})=\pmb{q}_i+\pmb{T}_i(\pmb{p}-\pmb{p}_i)
$$
和插值方程
$$
\pmb{f}(\pmb{p}_i)=\pmb{q}_i
$$
对每个i, 得线性方程组(下面略去下标i)
$$
\pmatrix{\pmb{A}&\pmb{O}\\\pmb{O}&\pmb{A}}
\pmatrix{\pmb{t}_1\\\pmb{t}_2}=
\pmatrix{\pmb{b}_1\\\pmb{b}_2}
$$
其中
$$
\pmb{T}=\pmatrix{\pmb{t}_1^T\\\pmb{t}_2^T}\\
\pmb{A}=\sum_{j\ne i}{\sigma_i(\pmb{p}_j)}
\pmatrix{\Delta \pmb{p}_x^2&\Delta\pmb{p}_x\Delta\pmb{p}_y\\
\Delta\pmb{p}_x\Delta\pmb{p}_y&\Delta\pmb{p}_y^2}\\
\pmb{b}_1=\sum_{j\ne i}\pmatrix{\Delta\pmb{p}_x\Delta\pmb{q}_x\\\Delta\pmb{p}_y\Delta\pmb{q}_x}\\
\pmb{b}_2=\sum_{j\ne i}\pmatrix{\Delta\pmb{p}_x\Delta\pmb{p}_y\\\Delta\pmb{p}_y\Delta\pmb{p}_y}\\
where\space \Delta\pmb{p}_x=\pmb{p}_{j,x}-\pmb{p}_{i,x},\,etc.
$$

#### RBF

$$
\pmb{f}(\pmb{p})=\sum \pmb{a}_i R_i(\|\pmb{p}-\pmb{p}_i\|)+\pmb{Mp}+\pmb{b}\\
R_i(d)=(d^2+r_i^2)^{\mu/2},\,\mu=\pm1
$$

其中, $\pmb{a}_i$含有$2N$个未知数, $\pmb{M}\in\R^{2\times 2}$和$\pmb{b}\in\R^2$含有6个未知数, 而插值方程$\pmb{f}(\pmb{p}_i)=\pmb{q}_i$至多能解$2N$个参数. 此外, 这里的$r_i$为任意满足$r_i>0$的实数, 我们选取$r_i=\min_{j\ne i}{\|\pmb{p}_i-\pmb{p}_j\|}$.

我们采用的方法为: 先通过所有点值计算出$\pmb{M}\in\R^{2\times 2}$和$\pmb{b}\in\R^2$, 使得仿射变换$\pmb{A}(\pmb{p})=\pmb{Mp}+\pmb{b}$在给定的$\pmb{p}_i,\pmb{q}_i$下有最小二乘误差, 即:
$$
\sum_i \|\pmb{A}(\pmb{p}_i)-\pmb{q}_i\|_2^2=min
$$
我们分以下三种情形讨论.

- 控制点只有一对($N=1$): 此时仿射变换默认为平移变换, 即$\pmb{M}=\pmb{I}_2$.

- 控制点有两对($N=2$): 此时仿射变换默认为平移和伸缩变换, 即$\pmb{M}=\pmb{Diag}(m_{11},m_{22})$.

- 控制点有三对以上($N\ge3$): 此时可以解出仿射变换的六个参数, 即解如下正规方程中的$\pmb\Theta$:
  $$
  \pmb{X}^T\pmb{X\Theta}=\pmb{X}^T\pmb{Y},\,where\\
  \pmb{X}=\pmatrix{1&\pmb{p}_1^T\\1&\pmb{p}_2^T\\..&..\\1&\pmb{p}_N^T}\in\R^{N\times3}\\
  \pmb\Theta=\pmatrix{\pmb b^T\\\pmb M^T}\in\R^{3\times2}\\
  \pmb{Y}=\pmatrix{\pmb q_1^T\\\pmb q_2^T\\..\\\pmb q_N^T\\}\in\R^{N\times2}
  $$
  

仿射变换确定后, 我们用这些点对来插值计算$\pmb a_i$. 即解如下方程中的$\pmb A$:
$$
\pmb G\pmb A=\pmb F,\,where\\
\pmb G=\pmatrix{R_j(\|\pmb p_i-\pmb p_j\|)}_{N\times N}\in\R^{N\times N}\\
\pmb A=\pmatrix{\pmb a_1^T\\..\\\pmb a_N^T}\in\R^{N\times2}\\
\pmb F=\pmb Y-\pmb X\pmb\Theta\in\R^{N\times2}
$$
从而确定了$\pmb f$.

#### Gouraud Shading

我们采取三角网格像素位置差值的方法来解决白缝填补的问题.

- **矩形网格划分**: 以用户设置的矩形网格边长进行均匀剖分(右、下边界除外, 保证矩形边长不大于设置的值. 边长默认值为$size=1+min/20$, 这里$min$表示图像较短边的长度. 没有图像时, 显示为-1.

- **三角形网格**: 每个矩形网格的左上角与右下角对角线将其分为两个三角形网格.

- **确定三角形内的点**: 首先将矩形网格的顶点在Warping函数下的像找到, 按序号进行查找. 然后对每个矩形的像找到其外接矩形(如图虚线部分), 对其内部的点进行检验. 即, 假设原矩形为$ABDC$, 对每个候选像点, 我们通过向量外积的正负性依次判断它是否在$\Delta A'B'C'$或$\Delta D'B'C'$内.

  ![image-20200301061304816](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301061304816.png)

- **插值**: 假设我们已经确定某一点$P'$在$\Delta A'B'C'$内. 我们计算出$P'$在$\{A';\vec{A'B'},\vec{A'C'}\}$下的坐标$(\alpha,\beta)$, 并推测其原像为$P=A+\alpha\vec{AB}+\beta\vec{AC}$, 将原图上对应该$P$位置的颜色赋值给$P'$.

- **局限性**: 对于潜在的**folding**问题, 这一方法可能会导出错误的结果. 如下图,  对于给定的$P'$, 我们的方法会计算得到左侧$\Delta ABC$中的$P$, 但实际上它的原像可能为$\Delta DBC$中的$P$.

  ![image-20200301062938118](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301062938118.png)

## Results

**Example 1 - IDW vs RBF**

- 原图

  ![image-20200301063528678](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301063528678.png)

- IDW

  ![image-20200301063616456](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301063616456.png)

- RBF

  ![image-20200301063628205](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301063628205.png)

**Example 2 - IDW vs RBF**

- 原图

  ![image-20200301064012586](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064012586.png)

- IDW

  ![image-20200301064153185](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064153185.png)

- RBF

  ![image-20200301064206855](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064206855.png)

**Example 3 - Gridsize: small vs large**

- 原图

  ![image-20200301064732184](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064732184.png)

  ![image-20200301064741386](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064741386.png)

- Gridsize = 1, RBF

  ![image-20200301064753391](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064753391.png)

- Gridsize = 20, RBF

  ![image-20200301064857685](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064857685.png)

- Gridsize = 50, RBF

  ![image-20200301064825463](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301064825463.png)

- Smaller grid size has better precision

**Example 4 - Gridsize: small vs large**

- 原图

  ![image-20200301065222124](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301065222124.png)

  ![image-20200301065952329](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301065952329.png)

- Gridsize=1, IDW

  ![image-20200301070518429](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301070518429.png)

- Gridsize=10, IDW

  ![image-20200301070548581](E:\2019-2020 Spring\CG\CG_Finished_Assignments\2_ImageWarping\report_img\image-20200301070548581.png)

- larger grid size has better smoothness and lower sensitivity to input datas

## Future work

- **navigation** of large image - currently we can only simply zoom in and zoom out, while users may need translate to navigate at a certain region of a large image
- undo points setting - we should **combine the input points and the image together as one class**, so that the warping points' history can also be stored and users could undo wrong points or even edit it directly.
- other methods of fixing white lines - **reverse mapping** and the use of **ANN**
- **allowing users to set two groups of points in RBF warping** - one for affine transformation, the other for the radius basis functions.
- **more settings** open to users - like the color of input lines, the parameter $u$ in both IDW and RBF warping, how to choose the affine transformation, etc.
- allow users to draw on the image or create a new image - **combine MiniDraw and ImageWarping**

