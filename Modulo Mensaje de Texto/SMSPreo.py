# Importamos el ModuMódulo

import pywhatkit 

# Usamos Un try-except
try: 

  # Enviamos el mensaje

  pywhatkit.sendwhatmsg("+5491161760026",  
                        "Pastilla retirada",
                        13,48) 

  print("Mensaje Enviado") 
  
except: 

  print("Ocurrio Un Error")