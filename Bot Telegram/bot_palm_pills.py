#!/usr/bin/env python
# pylint: disable=C0116,W0613
# This program is dedicated to the public domain under the CC0 license.
genero =0
nombre_apellido = 0
"""
First, a few callback functions are defined. Then, those functions are passed to
the Dispatcher and registered at their respective places.
Then, the bot is started and runs until we press Ctrl-C on the command line.

Usage:
Example of a bot-user conversation using ConversationHandler.
Send /start to initiate the conversation.
Press Ctrl-C on the command line or send a signal to the process to stop the
bot.
"""

import pymysql
import logging


from telegram import ReplyKeyboardMarkup, ReplyKeyboardRemove, Update
from telegram.ext import (
    Updater,
    CommandHandler,
    MessageHandler,
    Filters,
    ConversationHandler,
    CallbackContext,
    conversationhandler,
)

# Enable logging
logging.basicConfig(
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s', level=logging.INFO
)

logger = logging.getLogger(__name__)

GENDER, BIO, EDAD, ALARMA, CONFIG, HORA, NEW, HORA_1, NEXT, HORA_2, EDIT_1, SIGUIENTE, CHAU = range(13)

datos_db = []

seleccion_de_alarma=0
def start(update: Update, context: CallbackContext) -> int:
    """Starts the conversation and asks the user about their gender."""
    reply_keyboard = [['Hombre', 'Mujer', 'Otro']]

    update.message.reply_text(
        'Hola soy el bot de PalmPills. '
        'Estoy a tu disposición para cualquier consulta. Si es primera vez ingrese los datos del paciente: '
        'hombre o mujer u otro?',
        reply_markup=ReplyKeyboardMarkup(
            reply_keyboard, one_time_keyboard=True, input_field_placeholder='Hombre o Mujer u Otro?'
        ),
    )

    return GENDER
    
def gender(update: Update, context: CallbackContext) -> int:
    
    """Stores the selected gender and asks for a photo."""
    global datos_db

    paciente_genero = update.message.text    
    datos_db.append(paciente_genero)
    #logger.info("Gender of %s: %s", user.first_name, update.message.text)
    update.message.reply_text(
        'Nombre y Apellido: ',
        reply_markup=ReplyKeyboardRemove(),
    )

    return BIO

def location(update: Update, context: CallbackContext) -> int:
    """Stores the location and asks for some info about the user."""
    user = update.message.from_user
    user_location = update.message.location
    logger.info(
        "Location of %s: %f / %f", user.first_name, user_location.latitude, user_location.longitude
    )
    update.message.reply_text(
        'Maybe I can visit you sometime! At last, tell me something about yourself.'
    )

    return BIO


def skip_location(update: Update, context: CallbackContext) -> int:
    """Skips the location and asks for info about the user."""
    user = update.message.from_user
    logger.info("User %s did not send a location.", user.first_name)
    update.message.reply_text(
        'You seem a bit paranoid! At last, tell me something about yourself.'
    )

    return BIO


def bio(update: Update, context: CallbackContext) -> int:
    """Stores the info about the user and ends the conversation."""
    global datos_db

    nombre_paciente = update.message.text    
    datos_db.append(nombre_paciente)
    update.message.reply_text('Decime tu edad:')
    reply_markup=ReplyKeyboardRemove(),

    return EDAD

def edad(update: Update, context: CallbackContext) -> int:
    global datos_db

    pacientes_edad = update.message.text    
    datos_db.append(pacientes_edad)
    update.message.reply_text('Si es la primera vez, haz click /new, sino /edit para editar las alarmas')
    reply_markup=ReplyKeyboardRemove(),
    return NEW 

def new (update: Update, context: CallbackContext) -> int:
    #seleccion_alarma = update.message.text
    #update.message.reply_text('1 alarma o 2 alarma')
    reply_keyboard = [['1 Alarma', '2 Alarma']]
    update.message.reply_text(
        'Seleccione cual alarma va a programar:',
        reply_markup=ReplyKeyboardMarkup(
            reply_keyboard, one_time_keyboard=True, input_field_placeholder='1 alarma o 2 alarma'
        ),
    )
    return HORA

def hora (update: Update, context: CallbackContext) -> int:

    update.message.reply_text('Escriba la hora (Ejemplo: 18:10:00):')   
    return NEXT

def mensaje_siguiente (update: Update, context: CallbackContext) -> int:    
    update.message.reply_text('Perfecto!. Si quiere programar la siguiente/anterior pulse /alarma_2, sino /cancel para finalizar')
    reply_markup=ReplyKeyboardRemove(),
    return HORA_2

def hora_2 (update: Update, context: CallbackContext) -> int:
    update.message.reply_text('Escriba la segunda hora (Ejemplo: 18:10:00):')   
    reply_markup=ReplyKeyboardRemove(),
    return CHAU


def edit (update: Update, context: CallbackContext) ->int:
    # update.message.reply_text('Necesito que ingrese la hora de la alarma 1:')
    reply_keyboard = [['1 Alarma', '2 Alarma']]
    update.message.reply_text(
        'Seleccione cual alarma va a reprogramar:',
        reply_markup=ReplyKeyboardMarkup(
            reply_keyboard, one_time_keyboard=True, input_field_placeholder='1 alarma o 2 alarma'
        ),
    )
    return HORA
    

def chau (update: Update, context: CallbackContext) ->int:
    update.message.reply_text('Perfecto!, muchas gracias. Sigo a tu disposicion para cualquier consulta pulsa /ayuda.')
    reply_markup=ReplyKeyboardRemove(),
    return ConversationHandler.END

def ayuda (update: Update, context: CallbackContext) -> int:
    """Se llama el comando ayuda"""

    update.message.reply_text(
        'Estos son los comandos que te serviran de ayuda: \n'
        '/start \n'
        '/new \n'
        '/edit \n'
    )

    return ConversationHandler. END

def cancel(update: Update, context: CallbackContext) -> int:
    """Cancels and ends the conversation."""
    user = update.message.from_user
    logger.info("User %s canceled the conversation.", user.first_name)
    update.message.reply_text(
        'Perfecto!. Nos vemos pronto. Sigo a tu disposicion, cualquier consulta pulse /ayuda', reply_markup=ReplyKeyboardRemove()
    )

    return ConversationHandler.END


def main() -> None:

    con = pymysql.connect(
    host="localhost",
    user="root",
    password="",
    db="palmpills_db"
    )   
    cursor = con.cursor()

    sql = f"INSERT INTO pacientes (id, sexo, na, edad, alarmauno, alarmados) VALUES({2}, {genero}, {nombre_apellido}, {18}, {18:00:00}, {22:30:00})"
    cursor.execute(sql)
    con.commit()

    """Run the bot."""
    # Crea
    # te the Updater and pass it your bot's token.
    updater = Updater("1980081110:AAHkx7ffda6FzVXpToo71trrPDQZ9LAuOcs")

    # Get the dispatcher to register handlers
    dispatcher = updater.dispatcher

    # Add conversation handler with the states GENDER, PHOTO, LOCATION and BIO
    conv_handler = ConversationHandler(
        entry_points=[CommandHandler('start', start)],
        states={
            GENDER: [MessageHandler(Filters.regex('^(Hombre|Mujer|Otros)$'), gender)],
            BIO: [MessageHandler(Filters.text & ~Filters.command, bio)],
            EDAD: [MessageHandler(Filters.text & ~Filters.command, edad)],
            NEW: [CommandHandler('new', new),CommandHandler('edit', edit)],
            CONFIG: [CommandHandler('new', new)],
            HORA: [MessageHandler(Filters.text & ~Filters.command, hora)],
            NEXT: [MessageHandler(Filters.text & ~Filters.command, mensaje_siguiente)],
            HORA_2: [CommandHandler('alarma_2',hora_2)],
            CHAU: [MessageHandler(Filters.text & ~Filters.command, chau)],
        },
        fallbacks=[CommandHandler('cancel', cancel)],
        
    )
    conv_handler_2 = ConversationHandler(
        entry_points = [CommandHandler('ayuda', ayuda)],
        states = {
            0: [None],
        },
        fallbacks = [None]
    )

    conv_handler_new = ConversationHandler (
        entry_points = [CommandHandler('new', new)],
        states = {
            HORA: [MessageHandler(Filters.text & ~Filters.command, hora)],
            NEXT: [MessageHandler(Filters.text & ~Filters.command, mensaje_siguiente)],
            HORA_2: [CommandHandler('alarma_2',hora_2)],
            CHAU: [MessageHandler(Filters.text & ~Filters.command, chau)],
        },
        fallbacks=[CommandHandler('cancel', cancel)],
    )
    conv_handler_edit = ConversationHandler (
        entry_points = [CommandHandler('edit', edit)],
        states = {
            HORA: [MessageHandler(Filters.text & ~Filters.command, hora)],
            NEXT: [MessageHandler(Filters.text & ~Filters.command, mensaje_siguiente)],
            HORA_2: [CommandHandler('alarma_2',hora_2)],
            CHAU: [MessageHandler(Filters.text & ~Filters.command, chau)],
        },
        fallbacks=[CommandHandler('cancel', cancel)],
    )

    dispatcher.add_handler(conv_handler)
    dispatcher.add_handler(conv_handler_2)
    dispatcher.add_handler(conv_handler_new)
    dispatcher.add_handler(conv_handler_edit)
    # Start the Bot
    updater.start_polling()

    # Run the bot until you press Ctrl-C or the process receives SIGINT,
    # SIGTERM or SIGABRT. This should be used most of the time, since
    # start_polling() is non-blocking and will stop the bot gracefully.
    updater.idle()


if __name__ == '__main__':
    main()



    #tengo que correr el cursor con la funcion, y dentro del variables poner f adelante de todo y las comillas dentro de las comillas 
    #poner por ejemplo paciente_edad