## Adding your own data to LLMs

This project shows how to create a Chatbot that extends ChatGPT with your own data, using the RAG pattern with vector search. It shows three approaches to the problem: interacting directly with OpenAI APIs, using LangChain, and using Semantic Kernel. 


## Pre-requisites
- You need to have an Azure subscription. You can get a [free subscription](https://azure.microsoft.com/en-us/free) to try it out.
- Create a "Cognitive Search" resource on Azure.
- Create an "OpenAI" resource on Azure. Create two deployments within this resource: 
    - A deployment for the "text-embedding-ada-002" model.
    - A deployment for the "gpt-35-turbo" model.
- Add a ".env" file to the project with the following variables set:
    - **OPENAI_API_BASE** - Go to https://oai.azure.com/, "Chat", "View code", and find the API base in the code.
    - **OPENAI_API_KEY** - In the same window, copy the "Key" at the bottom.
    - **OPENAI_EMBEDDING_DEPLOYMENT** - Click on "Deployments" and find the name of the deployment for the "text-embedding-ada-002" model.
    - **OPENAI_CHATGPT_DEPLOYMENT** - In the same window, find the name of the deployment for the "gpt-35-turbo" model.
    - **AZURE_SEARCH_ENDPOINT** - Go to https://portal.azure.com/, find your "Cognitive Search" resource, and find the "Url".
    - **AZURE_SEARCH_KEY** - On the same resource page, click on "Settings", then "Keys", then copy the "Primary admin key".
    - **AZURE_SEARCH_SERVICE_NAME** - This is the name of the "Cognitive Search" resource in the portal.


## Install packages

Install the packages specified in the environment.yml file:

```
conda env create -f environment.yml
conda activate rag
```


## How to run

You can run the same scenario using one of three approaches:
- You can call the OpenAI APIs directly:
    - Run src/1_openai/init_search_1.py by opening the file and pressing F5. This initializes an Azure Cognitive Search index with our data.
    - Run src/1_openai/main_1.py. This runs a sequence of queries using our data.
- You can use the LangChain package:
    - Run src/2_langchain/init_search_2.py.
    - Run src/2_langchain/main_2.py.
- You can use the Semantic Kernel package:
    - Run src/3_semantic_kernel/init_search_3.py.
    - Run src/3_semantic_kernel/main_3.py.
