import cv2, numpy, copy, math, timeit, os, pickle
from Tkinter import *
from PIL import Image, ImageTk
import tkFileDialog

root = Tk()
slika_f = tkFileDialog.askopenfile(parent=root,mode='rb',title='Odaberite slikovnu datoteku')
root.destroy()
if (not slika_f):
    exit(1)
print("Program se pokrenuo...")    
slika_l = str(slika_f.name.split()[-1]).split('/')[-1].split('.')
slika_f.close()
start_time = timeit.default_timer()
slika = slika_l[0]
format_slk = '.'+slika_l[1]
print 'Ulazna slika: '+slika+format_slk
matrica_org_boja = cv2.imread('Input/'+slika+format_slk)
matrica = cv2.imread('Input/'+slika+format_slk)
matrica_slk = numpy.double(cv2.imread('Input/'+slika+format_slk, cv2.CV_LOAD_IMAGE_GRAYSCALE))

if not os.path.exists('Output/harris_temp/'):
    os.makedirs('Output/harris_temp/')

master = Tk()
frame = Frame(master)
frame.pack()
master.wm_title("Harrisovi kutovi")

sigma = 0.8
K = 0.05
velicina_prz = 12
prag = 20000000

broj_r = matrica_slk.shape[0]
broj_s = matrica_slk.shape[1]

matrica_orig = copy.copy(matrica_slk)
kernel_s = [[1,2,1],[1,0,-1],[1,0,-1],[1,2,1]] #X2, X1, Y2, Y1
matrica_pom1_lista = [[0 for s in range(broj_s)] for r in range(broj_r)]
matrica_pom2_lista = [[0 for s in range(broj_s)] for r in range(broj_r)]
for i in range(broj_r):
    if i%100 == 0:
        print '1. konvolucija, i',i, '/', broj_r
    for j in range(broj_s):
        for k in [-1, 0, 1]:
            X = j + k
            if X<0:
                X = broj_s - 1
            elif X>=broj_s:
                X = 2*broj_s - X - 1
            matrica_pom1_lista[i][j] += matrica_slk[i][X]*kernel_s[1][k+1]
            matrica_pom2_lista[i][j] += matrica_slk[i][X]*kernel_s[3][k+1]
gradx = [[0 for s in range(broj_s)] for r in range(broj_r)]
grady = [[0 for s in range(broj_s)] for r in range(broj_r)]
gradxy = [[0 for s in range(broj_s)] for r in range(broj_r)]
for i in range(broj_r-1):
    if i%100 == 0:
        print '2. konvolucija, i',i, '/', broj_r
    for j in range(broj_s-1):
        gx = 0
        gy = 0
        for k in [-1, 0, 1]:
            X = i + k
            if X<0:
                X = broj_r - 1
            elif X>=broj_r:
                
                X = 2*broj_r - X - 1
            gx += matrica_pom1_lista[X][j]*kernel_s[0][k+1]
            gy += matrica_pom2_lista[X][j]*kernel_s[2][k+1]
        gradx[i][j] = gx*gx
        grady[i][j] = gy*gy
        gradxy[i][j] = gx*gy

duljina_vkt = int(2*math.floor(sigma*3) + 1)
if sigma < 0:
    sigma = 0.3*((duljina_vkt - 1)*0.5 - 1) + 0.8
kernel_g = [0 for i in range(duljina_vkt)]
a = duljina_vkt/2
b = 0
suma_cln = 0
pi_sigma = 1/math.sqrt(2*math.pi*sigma*sigma)
while a>0:
    kernel_g[b] = kernel_g[duljina_vkt - b - 1] = pi_sigma*math.exp(-(a*a)/(2*sigma*sigma))
    suma_cln += 2*kernel_g[b]
    a-=1
    b+=1
kernel_g[b] = pi_sigma
suma_cln += pi_sigma
if suma_cln != 1:
    a = 0
    for a in range(duljina_vkt):
        kernel_g[a] /= suma_cln
    kernel_g[b] /= suma_cln
matrica_pom1_lista = [[0 for s in range(broj_s)] for r in range(broj_r)]
matrica_pom2_lista = [[0 for s in range(broj_s)] for r in range(broj_r)]
matrica_pom3_lista = [[0 for s in range(broj_s)] for r in range(broj_r)]

sx = [[0 for s in range(broj_s)] for r in range(broj_r)]
sy = [[0 for s in range(broj_s)] for r in range(broj_r)]
sxy = [[0 for s in range(broj_s)] for r in range(broj_r)]

for sklopka in [0,1]:
    for i in range(broj_r):
        if i%100 == 0:
            print 'Gauss '+str(sklopka+1)+'. korak, i',i , '/' , broj_r
        for j in range(broj_s):
            k = -duljina_vkt/2
            while k<duljina_vkt/2:
                pom1 = j + k
                if pom1<0:
                    pom1 = broj_s + k
                if pom1>=broj_s:
                    pom1 = 2*broj_s - pom1 - 1
                else:
                    if not (sklopka):
                        matrica_pom1_lista[i][j] += gradx[i][pom1]*kernel_g[k+duljina_vkt/2]
                        matrica_pom2_lista[i][j] += grady[i][pom1]*kernel_g[k+duljina_vkt/2]
                        matrica_pom3_lista[i][j] += gradxy[i][pom1]*kernel_g[k+duljina_vkt/2]
                    else:
                        sx[i][j] += matrica_pom1_lista[i][pom1]*kernel_g[k+duljina_vkt/2]
                        sy[i][j] += matrica_pom2_lista[i][pom1]*kernel_g[k+duljina_vkt/2]
                        sxy[i][j] += matrica_pom3_lista[i][pom1]*kernel_g[k+duljina_vkt/2]
                k+=1

lista = []
mapa_org = [[0 for s in range(broj_s)] for r in range(broj_r)]
for i in range(broj_r):
    if i%100 == 0:
        print 'Lista, i',i , '/' , broj_r
    for j in range(broj_s):        
        trag = sx[i][j] + sy[i][j]
        determ = sx[i][j]*sy[i][j] - sxy[i][j]*sxy[i][j]
        iznos = determ - K*trag*trag
        if iznos>prag:
            mapa_org[i][j] = iznos
            lista.append([i,j])
try:
    broj_kutova = pickle.load(open('Output/harris_temp/broj_kutova.pkl','rb'))
except:
    broj_kutova = {}
def dodavanje_kutovi(lista,velicina_prz_f):
    path = 'Output/harris_temp/'
    lista_fil = os.listdir(path)
    postoji = 0
    global prikaz_slk_tk
    for fil in lista_fil:
        if (slika in fil) and ('_'+str(velicina_prz_f)+'.' in fil):
            postoji = 1
            break
    if (postoji):
        prikaz_slk = Image.open('Output/harris_temp/'+slika+'_velicina_prz_'+str(velicina_prz_f)+format_slk)
        prikaz_slk_tk = ImageTk.PhotoImage(prikaz_slk)
        broj_k = broj_kutova[(slika,velicina_prz_f)]
        label2.configure(text='Broj kutova: '+broj_k)
        return
            
    ps=[]
    print (unos.get())
    
    mapa = []
    kutovi = []
    mapa = copy.copy(mapa_org)
    brj = 0
    for elem in lista:
        try:
            if brj%int(len(lista)/10) ==0:
                print 'Element u listi: ',lista.index(elem),'/',len(lista)
            brj+=1
        except:
            brj+=1
            pass
        x = elem[0]
        y = elem[1]
        iznos = mapa[x][y]
        zastavica = 1
        for j in range(-velicina_prz_f,velicina_prz_f):
            if not (zastavica):
                break
            for k in range(-velicina_prz_f,velicina_prz_f):
                if ((x + j)*broj_s + (y + k) > broj_r*broj_s):
                    continue
                if ((x + j)*broj_s + (y + k) < 0):
                    continue
                if (y+k<broj_s)and(x+j<broj_r)and(mapa[x+j][y+k]>iznos):
                    #mapa[x][y] = 0
                    zastavica = 0
                    break
                
        if (zastavica):
            kutovi.append([x,y])
    matrica_org_boja = copy.copy(matrica)
    matrica_org_tocke = copy.copy(matrica)*0
    for i in range(broj_r):
        for j in range(broj_s):
            matrica_org_tocke[i][j] = [255,255,255]

    for k in kutovi:
        try:
            for i in range(3):
                matrica_org_boja[k[0]-1-i][k[1]-1-i] = [0,0,255]
        except:
            pass
        try:
            for i in range(3):
                matrica_org_boja[k[0]-1-i][k[1]+1+i] = [0,0,255]
        except:
            pass
        try:
            for i in range(3):
                matrica_org_boja[k[0]+1+i][k[1]-1-i] = [0,0,255]
        except:
            pass
        try:
            for i in range(3):
                matrica_org_boja[k[0]+1+i][k[1]+1+i] = [0,0,255]
        except:
            pass 
        matrica_org_boja[k[0]][k[1]] = [0,0,255]
        matrica_org_tocke[k[0]][k[1]] = [0,0,0]
    cv2.imwrite('Output/harris_temp/'+slika+'_velicina_prz_'+str(velicina_prz_f)+format_slk,matrica_org_boja)
    print 'Slika '+ slika+'_harris_velicina_prz_'+str(velicina_prz_f)+format_slk+' je zapisana u Output/harris_temp/!'
    cv2.imwrite('Output/harris_temp/'+slika+'_tocke_velicina_prz_'+str(velicina_prz_f)+format_slk,matrica_org_tocke)
    prikaz_slk = Image.open('Output/harris_temp/'+slika+'_velicina_prz_'+str(velicina_prz_f)+format_slk)
    prikaz_slk_tk = ImageTk.PhotoImage(prikaz_slk)
    print str(len(kutovi))
    label2.configure(text='Broj kutova: '+str(len(kutovi)))
    broj_kutova[(slika,velicina_prz_f)] = str(len(kutovi))
    pickle.dump(broj_kutova,open('Output/harris_temp/broj_kutova.pkl','wb'))

def unos_ispis(event):
    global prikaz_slk_tk
    kutovi=[]
    dodavanje_kutovi(lista,int(unos.get()))
    panel.configure(image=prikaz_slk_tk)


label1 = Label(frame, text='Velicina prozora: ')
label1.pack(side=LEFT)
unos = Entry(frame, width=len(str(min(broj_r,broj_s)))+2)
unos.pack(side=LEFT)
label2 = Label(frame, text='Broj kutova: ')
label2.pack(side=LEFT)

panel = Label(master)
panel.pack()
unos.bind('<Return>',unos_ispis)
unos.insert(0,str(velicina_prz))

dodavanje_kutovi(lista,int(unos.get()))
panel.configure(image=prikaz_slk_tk)

elapsed = timeit.default_timer() - start_time
print 'Vrijeme izvodenja: '+str(elapsed)
b=raw_input()

mainloop()





