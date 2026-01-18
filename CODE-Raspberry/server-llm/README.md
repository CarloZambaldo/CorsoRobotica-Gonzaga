
1.  **Install Python dependencies:**

    ```bash
    pip install streamlit ollama
    ```

2.  **Ensure Ollama is running and the model is available:**

    If you haven't already, install Ollama and pull the model:

    ```bash
    # Install Ollama (refer to Ollama documentation for specific instructions for Raspberry Pi)
    # For example: curl -fsSL https://ollama.com/install.sh | sh

    ollama pull gemma3:1b-it-qat
    ```

### Running the Application

1.  **Start the Streamlit application:**

    ```bash
    streamlit run app.py
    