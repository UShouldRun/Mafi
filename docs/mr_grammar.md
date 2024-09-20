$$

\begin{align}

<\text{.mr}> &\to <\text{depth}>\,\text{\textbackslash n}\,<\text{Rules}>\,\text{\textbackslash n}\,<\text{Timetable\_info}>\,\text{\textbackslash n}\,<\text{Classes}>\,\text{\textbackslash n}\,<\text{Users}> \\

\\

<\text{depth}> &\to <\text{size\_t}> \\

\\

<\text{Rules}> &\to <\text{r1}>\,<\text{r2}>\,<\text{r3}>\,<\text{r4}>\,<\text{r5}>\,<\text{r6}> \\
<\text{r1}> &\to <\text{max\_students\_per\_classroom}>\to <\text{size\_t}> \\
<\text{r2}> &\to <\text{max\_teachers\_per\_classroom}>\to <\text{size\_t}> \\
<\text{r3}> &\to <\text{max\_same\_class\_per\_week\_per\_student}>\to <\text{size\_t}> \\
<\text{r4}> &\to <\text{max\_classes\_per\_classroom}>\to <\text{size\_t}> \\
<\text{r5}> &\to <\text{max\_blocks\_per\_day\_per\_user}>\to <\text{size\_t}> \\
<\text{r6}> &\to <\text{morning\_afternoon}>\to 0 \mid 1 \\

\\

<\text{Timetable\_info}> &\to <\text{days}>\,<\text{space}>\,<\text{blocks\_per\_day}>\,\text{\textbackslash n}\,<\text{Classrooms}> \text{\textbackslash n} \\
<\text{Classrooms}> &\to <\text{number\_of\_classrooms}>\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,<\text{classroom}>\,\text{\textbackslash n}\,\}^{<\text{number\_of\_classrooms}>} \\
<\text{days}> &\to <\text{size\_t}> \\
<\text{blocks\_per\_day}> &\to <\text{size\_t}> \\
<\text{number\_of\_classrooms}> &\to <\text{size\_t}> \\
<\text{classroom}> &\to <\text{id}> \\

\\

<\text{Classes}> &\to <\text{size\_classes}>\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,<\text{class}>\,\text{\textbackslash n}\,<\text{Locations}>\,\}^{<\text{size\_classes}>} \\
<\text{Locations}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,<\text{size\_locations}>\,\{\,\text{\textbackslash n}\,<\text{Location}>\,\text{\textbackslash n}\,\}^{<\text{size\_locations}>} \\
<\text{Location}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{Entry}>\,<\text{space}>\,<\text{classroom}> \\
<\text{Entry}> &\to <\text{i}>\,<\text{space}>\,<\text{j}> \\
<\text{class}> &\to <\text{id}> \\
<\text{size\_classes}> &\to <\text{size\_t}> \\
<\text{size\_locations}> &\to <\text{size\_t}> \\
<\text{i}> &\to <\text{size\_t}> \\
<\text{j}> &\to <\text{size\_t}> \\

\\

<\text{Users}> &\to <\text{size\_users}>\,\text{\textbackslash n}\,\{\,<\text{User}>\,\text{\textbackslash n}\,\}^{<\text{size\_users}>} \\
<\text{User}> &\to \text{\textbackslash t}\,<\text{status}>\,<\text{space}>\,<\text{id}>\,\text{\textbackslash n}\,<\text{User\_classes}>\,<\text{User\_availability}> \\
<\text{User\_classes}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,<\text{size\_user\_classes}>\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{class}>\,\text{\textbackslash n}\,\}^{<\text{size\_user\_classes}>} \\
<\text{User\_availability}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,<\text{size\_user\_avail}>\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{entry}>\,\text{\textbackslash n}\,\}^{<\text{size\_user\_avail}>} \\
<\text{status}> &\to 1 \mid 2 \mid 3 \\
<\text{size\_users}> &\to <\text{size\_t}> \\
<\text{size\_user\_classes}> &\to <\text{size\_t}> \\
<\text{size\_user\_avail}> &\to <\text{size\_t}> \\

\\

<\text{id}> &\to <\text{uint}> \\
<\text{size\_t}> &\to <\text{Non-negative integer (unsigned integer)}> \\
<\text{uint}> &\to <\text{Unsigned integer}> \\
    
\end{align}

$$