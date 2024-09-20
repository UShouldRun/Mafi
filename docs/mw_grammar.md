$$

\begin{align}

<\text{.mw}> &\to <\text{size\_timetables}>\,\text{\textbackslash n}\,\{\,<\text{Timetable}>\,\}^{\,<\text{size\_timetables}>\,} \\
<\text{Timetable}> &\to <\text{n}>\,<\text{space}>\,<\text{m}>\,\text{\textbackslash n}\,\{\,<\text{Entry}>\,\}^{\,<\text{n}>\,\text{*}\,<\text{m}>\,} \\
<\text{Entry}> &\to \text{\textbackslash t}\,<\text{i}>\,<\text{space}>\,<\text{j}>\,<\text{space}>\,<\text{size\_classrooms}>\,\text{\textbackslash n}\,\{\,<\text{Classroom}>\,\}^{<\text{size\_classrooms}>}\,\text{\textbackslash n} \\
<\text{Classroom}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,<\text{id}>\,<\text{space}>\,<\text{size\_classes}>\,\text{\textbackslash n}\,\{\,<\text{Class}>\,\}^{<\text{size\_classes}>}\,\text{\textbackslash n} \\
<\text{Class}> &\to \text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{id}>\,<\text{space}>\,<\text{size\_students}>\,<\text{space}>\,<\text{size\_teachers}>\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{student}>\,\text{\textbackslash n}\,\}^{\text{size\_students}}\,\text{\textbackslash n}\,\{\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,\text{\textbackslash t}\,<\text{teacher}>\,\text{\textbackslash n}\,\}^{\text{size\_teachers}}\,\text{\textbackslash n} \\
\\
<\text{size\_timetables}> &\to <\text{size\_t}> \\
<\text{n}> &\to <\text{size\_t}> \\
<\text{m}> &\to <\text{size\_t}> \\
<\text{i}> &\to <\text{size\_t}> \\
<\text{j}> &\to <\text{size\_t}> \\
<\text{size\_classrooms}> &\to <\text{size\_t}> \\
<\text{size\_classes}> &\to <\text{size\_t}> \\
<\text{size\_students}> &\to <\text{size\_t}> \\
<\text{size\_teachers}> &\to <\text{size\_t}> \\
\\
<\text{student}> &\to <\text{id}> \\
<\text{teacher}> &\to <\text{id}> \\
<\text{id}> &\to <\text{uint}> \\
 
\end{align}

$$