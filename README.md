University of Zagreb<br>
Faculty of Electrical Engineering and Computing

# Software Design Project

<a href="http://www.fer.unizg.hr">http://www.fer.unizg.hr</a>

2014./2015.

# Project theme: Elementary differential monitoring process (Kanade–Lucas–Tomasi feature tracker)


(c) 2015 Kristijan Biščanić, Matej Djerdji, Ivan Fabijanić, Josip Milić, Dario Pažin

*Version 1.1*

Date: **15.01.2015.**<br>

###Project description:
####Elementary differential monitoring process is a motion analysis problem of certain features in a series of pictures. 
####Offsetting image is approximated by the Taylor development. In this development, the partial derivatives of image coordinates movements (or gradient image) occur, and because of this procedure is called differential.

More information in the documentation (warning: written in Croatian): https://github.com/josip-milic/KLT-feature-tracker/blob/master/Tehni%C4%8Dka%20dokumentacija/FER2_Projekt_Tehnicka_Dokumentacija_-_Elementarni_postupak_diferencijalnog_pracenja.pdf


Programming languages and its packages used in this project (more information in the documentation, chapter 8):
<ul>
	<li>
		Python 2.7 - Prototyping<br/>
		Used external packages:
		<ul>
			<li>NumPy - N-dimensional arrays and linear algebra</li>
			<li>OpenCV - Computer vision methods (mostly for image/video import and export)</li>
			<li>PIL - User interface</li>
		</ul>
	</li>
	<li>
		C++ - Final program<br/>
		Used external packages:
		<ul>
			<li>OpenCV - Computer vision methods (mostly for image/video import and export)</li>
		</ul>
	</li>
</ul>
