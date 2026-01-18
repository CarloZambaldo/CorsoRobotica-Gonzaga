import streamlit as st
import ollama

st.set_page_config(page_title="Raspberry Pi Local GPT", page_icon="🤖")

st.title("🤖 RPi Local GPT")
st.markdown("Interfaccia gestita da **Nginx** e **Streamlit**")

# Inizializza la cronologia della chat
if "messages" not in st.session_state:
    st.session_state.messages = []

# Visualizza i messaggi precedenti
for message in st.session_state.messages:
    with st.chat_message(message["role"]):
        st.markdown(message["content"])

# Input dell'utente
if prompt := st.chat_input("Chiedi qualcosa al tuo Raspberry..."):
    st.session_state.messages.append({"role": "user", "content": prompt})
    with st.chat_message("user"):
        st.markdown(prompt)

    # Risposta del modello
    with st.chat_message("assistant"):
        response_placeholder = st.empty()
        full_response = ""
        
        # Chiamata stream a Ollama
        response = ollama.chat(model='gemma3:1b-it-qat', messages=st.session_state.messages, stream=True)
        
        for chunk in response:
            full_response += chunk['message']['content']
            response_placeholder.markdown(full_response + "▌")
        
        response_placeholder.markdown(full_response)
    
    st.session_state.messages.append({"role": "assistant", "content": full_response})

