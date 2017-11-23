% -------------------------------------------------------------------
%  @LaTeX-class-file{
%     filename        = "Dissertate.cls",
%     version         = "2.0",
%     date            = "25 March 2014",
%     codetable       = "ISO/ASCII",
%     keywords        = "LaTeX, Dissertate",
%     supported       = "Send email to suchow@post.harvard.edu.",
%     docstring       = "Class for a dissertation."
% --------------------------------------------------------------------

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{Dissertate}[2014/03/25 v2.0 Dissertate Class]
\LoadClass[12pt, twoside, openright, a4paper]{book}

%
% Options
%
\RequirePackage{etoolbox}

% Line spacing: dsingle/ddouble
%   Whether to use single- or doublespacing.
\newtoggle{DissertateSingleSpace}
\toggletrue{DissertateSingleSpace}
\DeclareOption{dsingle}{
    \toggletrue{DissertateSingleSpace}
    \ClassWarning{Dissertate}{Single-spaced mode on.}
}
\DeclareOption{ddouble}{\togglefalse{DissertateSingleSpace}}

\ProcessOptions\relax

% Line Spacing
%   Define two line spacings: one for the body, and one that is more compressed.
\iftoggle{DissertateSingleSpace}{
    \newcommand{\dnormalspacing}{1.5}
    \newcommand{\dcompressedspacing}{1.0}
}{
    \newcommand{\dnormalspacing}{2.0}
    \newcommand{\dcompressedspacing}{1.2}
}

% Block quote with compressed spacing
\let\oldquote\quote
\let\endoldquote\endquote
\renewenvironment{quote}
    {\begin{spacing}{\dcompressedspacing}\oldquote}
    {\endoldquote\end{spacing}}

% Itemize with compressed spacing
\let\olditemize\itemize
\let\endolditemize\enditemize
\renewenvironment{itemize}
    {\begin{spacing}{\dcompressedspacing}\olditemize}
    {\endolditemize\end{spacing}}

% Enumerate with compressed spacing
\let\oldenumerate\enumerate
\let\endoldenumerate\endenumerate
\renewenvironment{enumerate}
    {\begin{spacing}{\dcompressedspacing}\oldenumerate}
    {\endoldenumerate\end{spacing}}

% Images with compressed spacing
\let\oldenumerate\figure
\let\endoldenumerate\endfigure
\renewenvironment{figure}
    {\begin{spacing}{\dcompressedspacing}\oldenumerate}
    {\endoldenumerate\end{spacing}}

% Text layout.
\RequirePackage[outer=1in, inner=1.55in, twoside, a4paper]{geometry}
\usepackage{ragged2e}
%\RaggedRight
\RequirePackage{graphicx}
\usepackage{fixltx2e}
\parindent 12pt
\RequirePackage{lettrine}
\RequirePackage{setspace}
\RequirePackage{verbatim}

% Fonts.
\RequirePackage{color}
\RequirePackage{xcolor}
\usepackage{hyperref}
\RequirePackage{url}
\RequirePackage{amssymb}
\RequirePackage{amsmath}
\RequirePackage{mathspec}
%\setmathsfont(Digits,Latin,Greek)[Numbers={Proportional}]{EB Garamond}
%\setmathrm{EB Garamond}
\AtBeginEnvironment{tabular}{\addfontfeature{RawFeature=+tnum}}
\widowpenalty=300
\clubpenalty=300
\setromanfont[Numbers=OldStyle, Ligatures={Common, TeX}, Contextuals=NoAlternate, Scale=1.0]{EB Garamond}
\newfontfamily{\smallcaps}[RawFeature={+c2sc,+scmp}]{EB Garamond}
\setsansfont[Scale=MatchLowercase, BoldFont={Lato Bold}]{Lato Regular}
\setmonofont[Scale=MatchLowercase]{Source Code Pro}
\RequirePackage[labelfont={bf,sf,footnotesize,singlespacing},
                textfont={sf,footnotesize,singlespacing},
                justification={justified,RaggedRight},
                singlelinecheck=false,
                margin=0pt,
                figurewithin=chapter,
                tablewithin=chapter]{caption}
\renewcommand{\thefootnote}{\fnsymbol{footnote}}
\RequirePackage{microtype}

% Tikz
\usepackage{tikz}
\usepackage{pgfplots}
\pgfplotsset{compat=newest}
\pgfplotsset{plot coordinates/math parser=false}
\newlength\fheight
\newlength\fwidth
\usepackage{booktabs}
\usepackage{multirow}
%\usepackage{multicolumn}
\usepackage{subcaption}
\usetikzlibrary{patterns,decorations.pathreplacing,backgrounds,calc}
\usepackage{afterpage}
\usepackage{tabulary}
\newcommand{\ra}[1]{\renewcommand{\arraystretch}{#1}}


% Pseudocode
\usepackage{algorithm}
\usepackage[noend]{algpseudocode}
\renewcommand\algorithmicthen{}
\renewcommand\algorithmicdo{}
\@addtoreset{algorithm}{chapter}% algorithm counter resets every chapter
\usepackage{lscape}
\renewcommand{\thealgorithm}{\thechapter.\arabic{algorithm}}% Algorithm # is <chapter>.<algorithm>


% Abbreviations
\newcommand{\abbrlabel}[1]{\makebox[3cm][l]{\textbf{#1}\ \dotfill}}
\newenvironment{abbreviations}{\begin{list}{}{\renewcommand{\makelabel}{\abbrlabel}}}{\end{list}}


% Headings and headers.
\RequirePackage{emptypage}
\RequirePackage{fancyhdr}
\RequirePackage[tiny, md, sc]{titlesec}
\setlength{\headheight}{15pt}
% \pagestyle{myheadings}
\pagestyle{fancy}
\renewcommand{\chaptermark}[1]{%
 \markboth{\chaptername
 \ \thechapter.\ #1}{}}
\renewcommand{\sectionmark}[1]{\markright{Sezione \thesection.\ #1}{}}
% E: Even page, O: Odd page, L: Left field, C: Center field
% R: Right field, H: Header, F: Footer
\fancyhead{} % clear all header fields
\fancyhead[RE]{\leftmark}
\fancyhead[LO]{\rightmark}
\fancyhead[RO,LE]{\thepage}
\fancyfoot{} % clear all footer fields
% \fancyfoot[CE,CO]{\thepage}
% \fancyfoot[CO,CE]{Marco Romanelli}
\renewcommand{\headrulewidth}{0.4pt}
\renewcommand{\footrulewidth}{0pt}

\RequirePackage{titling}

% Front matter.
\setcounter{tocdepth}{2}
\usepackage[titles]{tocloft}
\usepackage[titletoc]{appendix}
\renewcommand{\cftsecleader}{\cftdotfill{\cftdotsep}}
\renewcommand{\cftchapfont}{\normalsize \scshape}
\renewcommand\listfigurename{Elenco delle figure}
\renewcommand\listtablename{Elenco delle tabelle}

% Endmatter
\renewcommand{\setthesection}{\arabic{chapter}.A\arabic{section}}

% References.
\renewcommand\bibname{Bibliografia}
%\RequirePackage[super,comma,numbers]{natbib}
\RequirePackage[comma,numbers]{natbib}
\renewcommand{\bibnumfmt}[1]{[#1]}
\RequirePackage[palatino]{quotchap}
\renewcommand*{\chapterheadstartvskip}{\vspace*{-0.5\baselineskip}}
\renewcommand*{\chapterheadendvskip}{\vspace{1.3\baselineskip}}
\bibliographystyle{../utils/IEEEtran_ita.bst}

% An environment for paragraph-style section.
\providecommand\newthought[1]{%
   \addvspace{1.0\baselineskip plus 0.5ex minus 0.2ex}%
   \noindent\textsc{#1}%
}

% Align reference numbers so that they do not cause an indent.
\newlength\mybibindent
\setlength\mybibindent{0pt}
\renewenvironment{thebibliography}[1]
    {\chapter*{\bibname}%
     \@mkboth{\MakeUppercase\bibname}{\MakeUppercase\bibname}%
     \list{\@biblabel{\@arabic\c@enumiv}}
          {\settowidth\labelwidth{\@biblabel{999}}
           \leftmargin\labelwidth
            \advance\leftmargin\dimexpr\labelsep+\mybibindent\relax\itemindent-\mybibindent
           \@openbib@code
           \usecounter{enumiv}
           \let\p@enumiv\@empty
           \renewcommand\theenumiv{\@arabic\c@enumiv}}
     \sloppy
     \clubpenalty4000
     \@clubpenalty \clubpenalty
     \widowpenalty4000%
     \sfcode`\.\@m}
    {\def\@noitemerr
      {\@latex@warning{Empty `thebibliography' environment}}
     \endlist}

% Some definitions.
\def\advisor#1{\gdef\@advisor{#1}}
\def\mastername#1{\gdef\@mastername{#1}}
\def\coadvisorOne#1{\gdef\@coadvisorOne{#1}}
\def\coadvisorOneUniversity#1{\gdef\@coadvisorOneUniversity{#1}}
\def\coadvisorTwo#1{\gdef\@coadvisorTwo{#1}}
\def\committeeInternal#1{\gdef\@committeeInternal{#1}}
\def\committeeInternalOne#1{\gdef\@committeeInternalOne{#1}}
\def\committeeInternalTwo#1{\gdef\@committeeInternalTwo{#1}}
\def\committeeExternal#1{\gdef\@committeeExternal{#1}}
\def\degreeyear#1{\gdef\@degreeyear{#1}}
\def\degreemonth#1{\gdef\@degreemonth{#1}}
\def\degreeterm#1{\gdef\@degreeterm{#1}}
\def\degree#1{\gdef\@degree{#1}}
\def\department#1{\gdef\@department{#1}}
\def\field#1{\gdef\@field{#1}}
\def\university#1{\gdef\@university{#1}}
\def\universitycity#1{\gdef\@universitycity{#1}}
\def\universitystate#1{\gdef\@universitystate{#1}}
\def\programname#1{\gdef\@programname{#1}}
\def\pdOneName#1{\gdef\@pdOneName{#1}}
\def\pdOneSchool#1{\gdef\@pdOneSchool{#1}}
\def\pdOneYear#1{\gdef\@pdOneYear{#1}}
\def\pdTwoName#1{\gdef\@pdTwoName{#1}}
\def\pdTwoSchool#1{\gdef\@pdTwoSchool{#1}}
\def\pdTwoYear#1{\gdef\@pdTwoYear{#1}}
% School name and location
\university{University of Padova}
\universitycity{Padova}
\universitystate{Italy}
\def\academicyear#1{\gdef\@academicyear{#1}}

% School color found from university's graphic identity site:
% http://www.nyu.edu/employees/resources-and-services/media-and-communications/styleguide.html
\definecolor{SchoolColor}{rgb}{0.71, 0, 0.106} % UNIPD red
\definecolor{chaptergrey}{rgb}{0.61, 0, 0.09} % dialed back a little
\definecolor{midgrey}{rgb}{0.4, 0.4, 0.4}

\hypersetup{
    colorlinks,
    citecolor=SchoolColor,
    filecolor=black,
    linkcolor=black,
    urlcolor=SchoolColor,
}


\renewcommand{\frontmatter}{
	% \pagenumbering{roman}
	\pagestyle{empty}
	\input{frontmatter/personalize}
	\maketitle
	\frontispiece
	\dedicationpage
	\pagestyle{fancy}
	\abstractpage
	% \abstract
	\tableofcontents
	\phantomsection
	\clearpage

	% figure listing - required if you have any figures
	\addcontentsline{toc}{chapter}{Elenco delle figure}
	\listoffigures
	\phantomsection
	\cleardoublepage

	% table listing - required if you have any tables
	\addcontentsline{toc}{chapter}{Elenco delle tabelle}
	\listoftables
	\phantomsection
	\cleardoublepage

	% algorithms listing - required if you have any algorithms
	\addcontentsline{toc}{chapter}{Elenco degli algoritmi}
	\listofalgorithms
	\phantomsection
	\cleardoublepage

	% % table listing - required if you have any tables
	% \addcontentsline{toc}{chapter}{Frammenti di codice}
	% \lstlistoflistings
	% \phantomsection
	% \cleardoublepage

	\acronyms

	\cleardoublepage
	\setcounter{page}{1}
	\pagenumbering{arabic}
}

\newcommand{\cover}{
	\pagenumbering{roman}
	\input{frontmatter/personalize}
	\makecover
}

\renewcommand{\maketitle}{
	\thispagestyle{empty}
	\setcounter{page}{1}
	\begin{center}
	\vbox to0pt{\vbox to\textheight{\vfill \includegraphics[width=11.5cm]{resources/unipd-light} \vfill}\vss}
	%\vspace*{\fill}
	\begin{figure}
	\centering
  		\includegraphics[height=2.5cm]{resources/unipd-bn}
	\end{figure}

	\setstretch{1.5}

	\scshape{\Large{Università degli Studi di Padova}} \\
	{\color{SchoolColor}\line(1, 0){400}} \\
	\scshape{\large{Dipartimento di Matematica ``Tullio Levi-Civita''}} \\

	\vspace{5pt}
	\small{\textit{Corso di}}
	\@mastername

	\setstretch{2.5}

	\vspace{30pt}
	\scshape{\huge{\bfseries{\textcolor{SchoolColor}{\thetitle}}}} \normalsize \\
	\vspace{25pt}
	\vspace{15pt}

	\setstretch{1.2}

	% \vfill
	\vspace{100pt}
	\begin{normalsize}
	\begin{flushleft}
	  \textit{Relatore} \hfill \textit{Candidato}\\
	  \vspace{1pt}
	  \@advisor \hfill \@author\\
	  % Università di Padova \\
	  \vspace{6pt}
	  \textit{Co-relatore} \\
	  \@coadvisorOne \\
	  % \@coadvisorOneUniversity
	  % \vspace{10pt}
	\end{flushleft}
	\end{normalsize}

	\vspace{30pt}
	\small{Anno Accademico \@academicyear} \\

	\end{center}
	\vspace*{\fill}
	\singlespacing
	\cleardoublepage
}

\newcommand{\copyrightpage}{
	\newpage
	\thispagestyle{empty}
	\vspace*{25pt}
	\begin{center}
	\scshape \noindent \small \copyright \  \small  \theauthor \\
	all rights reserved, \@degreeyear
	\end{center}
	\newpage
	\rm
}

\newcommand{\frontispiece}{
	\phantomsection
	\thispagestyle{empty}
	\newpage
	\vspace*{\fill}
	\begin{center}
	\end{center}
	\vspace*{\fill}
}

\newcommand{\dedicationpage}{
	\phantomsection
	\setcounter{page}{3}
	\vspace*{\fill}
	\scshape \noindent \input{frontmatter/dedication}
	\vspace*{\fill}
	\cleardoublepage
	\rm
}

\newcommand{\acknowledgments}{
	\phantomsection
	\addcontentsline{toc}{chapter}{Ringraziamenti}
	\chapter*{Ringraziamenti}
	\noindent
	\input{frontmatter/thanks}
	\vspace*{\fill} \newpage
}

\newcommand{\abstract}{
	\phantomsection
	\chapter*{Abstract}
	\input{frontmatter/preface}
}

\newcommand{\acronyms}{
	\phantomsection
	\addcontentsline{toc}{chapter}{Elenco degli acronimi}
	\chapter*{Elenco degli acronimi}
	\input{frontmatter/abbr.tex}
}

\newcommand{\abstractpage}{
	\phantomsection
	\pagenumbering{roman}
	\setcounter{page}{5}
	\addcontentsline{toc}{chapter}{Abstract}
	\chapter*{Abstract}
	\input{frontmatter/preface}
}


\renewcommand{\backmatter}{
    \begin{appendices}
        \include{chapters/appendixA}
    \end{appendices}
    \input{endmatter/personalize}
    \clearpage
    \bibliography{references}
    \addcontentsline{toc}{chapter}{Bibliografia}
    \bibliographystyle{apalike2}
    \include{endmatter/colophon}
}
