"""Iconos geométricos propios: siete PNG RGBA 32x32, generados con Pillow."""
from pathlib import Path
from PIL import Image, ImageDraw
DESTINO = Path(__file__).resolve().parents[1] / 'assets' / 'icons'
DESTINO.mkdir(parents=True, exist_ok=True)

def generar(nombre, dibujar):
    imagen = Image.new('RGBA', (128,128))
    original = ImageDraw.Draw(imagen)
    class Pincel:
        def __getattr__(self, metodo):
            def trazar(puntos, **opciones):
                for clave in ('width','radius'):
                    if clave in opciones: opciones[clave] *= 4
                puntos = [(x*4,y*4) for x,y in puntos] if isinstance(puntos[0],tuple) else [v*4 for v in puntos]
                return getattr(original,metodo)(puntos,**opciones)
            return trazar
    dibujar(Pincel())
    imagen.resize((32,32),Image.Resampling.LANCZOS).save(DESTINO / (nombre+'.png'))

def vehiculo(p,color,medico=False,escalera=False):
    p.rounded_rectangle((2,8,29,25),radius=3,fill=color,outline='#17324D',width=1)
    p.rectangle((20,10,26,16),fill='#D7F0FF')
    p.rectangle((10,5,17,8),fill='#F54B56')
    for x in (7,24): p.ellipse((x-3,22,x+3,28),fill='#17324D',outline='white',width=1)
    if medico:
        p.rectangle((9,11,12,21),fill='#D62839')
        p.rectangle((6,14,15,17),fill='#D62839')
    elif escalera:
        p.rectangle((5,11,16,20),outline='white',width=1)
        for y in (14,17): p.line((5,y,16,y),fill='white',width=1)
    else: p.polygon([(10,12),(15,14),(14,19),(10,22),(6,19),(5,14)],fill='white')

def urgencia(p):
    p.ellipse((2,2,30,30),fill='white',outline='#D62839',width=2)
    p.rectangle((13,7,19,25),fill='#D62839')
    p.rectangle((7,13,25,19),fill='#D62839')

def incendio(p):
    p.polygon([(16,1),(20,11),(25,7),(29,19),(27,27),(21,31),(10,31),(4,26),(3,19),(10,9),(10,18)],fill='#E85D04',outline='#9D3600')
    p.polygon([(16,13),(23,23),(21,28),(12,28),(10,23)],fill='#FFD166')

def accidente(p):
    p.polygon([(16,2),(31,29),(1,29)],fill='#FFD166',outline='#6B4900',width=2)
    p.rounded_rectangle((14,11,18,21),radius=1,fill='#513800')
    p.ellipse((14,24,18,28),fill='#513800')

def robo(p):
    p.polygon([(16,1),(29,6),(27,21),(22,27),(16,31),(10,27),(5,21),(3,6)],fill='#2864A5',outline='#17324D')
    p.polygon([(16,7),(18,13),(24,13),(19,17),(21,23),(16,19),(11,23),(13,17),(8,13),(14,13)],fill='white')

generar('ambulancia',lambda p:vehiculo(p,'#F8FAFC',medico=True))
generar('bombero',lambda p:vehiculo(p,'#CF3542',escalera=True))
generar('serenazgo',lambda p:vehiculo(p,'#2864A5'))
for nombre,funcion in [('urgencia_medica',urgencia),('incendio',incendio),('accidente',accidente),('robo',robo)]: generar(nombre,funcion)
