/*
 * Web Services on Devices
 * XML tests
 *
 * Copyright 2017 Owen Rudge for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define COBJMACROS

#include <windows.h>

#include "wine/test.h"
#include "wsdapi.h"

static LPWSTR largeText;
static const int largeTextSize = 10000 * sizeof(WCHAR);

static void BuildAnyForSingleElement_tests(void)
{
    WSDXML_ELEMENT *element;
    WSDXML_NAME name;
    WSDXML_NAMESPACE ns;
    WCHAR nameText[] = {'E','l','1',0};
    WCHAR text[] = {'H','e','l','l','o',0};
    static const WCHAR uri[] = {'h','t','t','p',':','/','/','t','e','s','t','.','t','e','s','t','/',0};
    static const WCHAR prefix[] = {'t',0};
    HRESULT hr;

    /* Populate structures */
    ns.Uri = uri;
    ns.PreferredPrefix = prefix;

    name.LocalName = nameText;
    name.Space = &ns;

    /* Test invalid arguments */
    hr = WSDXMLBuildAnyForSingleElement(NULL, NULL, NULL);
    ok(hr == E_INVALIDARG, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&name, NULL, NULL);
    ok(hr == E_POINTER, "BuildAnyForSingleElement failed with %08x\n", hr);

    /* Test calling the function with a text size that exceeds 8192 characters */
    hr = WSDXMLBuildAnyForSingleElement(&name, largeText, &element);
    ok(hr == E_INVALIDARG, "BuildAnyForSingleElement failed with %08x\n", hr);

    /* Test with valid parameters but no text */

    hr = WSDXMLBuildAnyForSingleElement(&name, NULL, &element);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    ok(element->Name != &name, "element->Name has not been duplicated\n");
    todo_wine ok(element->Name->Space != name.Space, "element->Name->Space has not been duplicated\n");
    ok(element->Name->LocalName != name.LocalName, "element->LocalName has not been duplicated\n");
    ok(lstrcmpW(element->Name->LocalName, name.LocalName) == 0, "element->LocalName = %s, expected %s\n",
        wine_dbgstr_w(element->Name->LocalName), wine_dbgstr_w(name.LocalName));
    ok(element->FirstChild == NULL, "element->FirstChild == %p\n", element->FirstChild);
    ok(element->Node.Next == NULL, "element->Node.Next == %p\n", element->Node.Next);
    ok(element->Node.Type == ElementType, "element->Node.Type == %d\n", element->Node.Type);

    WSDFreeLinkedMemory(element);

    /* Test with valid parameters and text */

    hr = WSDXMLBuildAnyForSingleElement(&name, text, &element);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    ok(element->Name != &name, "element->Name has not been duplicated\n");
    todo_wine ok(element->Name->Space != name.Space, "element->Name->Space has not been duplicated\n");
    ok(element->Name->LocalName != name.LocalName, "element->LocalName has not been duplicated\n");
    ok(lstrcmpW(element->Name->LocalName, name.LocalName) == 0, "element->LocalName = %s, expected %s\n",
        wine_dbgstr_w(element->Name->LocalName), wine_dbgstr_w(name.LocalName));
    ok(element->FirstChild != NULL, "element->FirstChild == %p\n", element->FirstChild);
    ok(element->Node.Next == NULL, "element->Node.Next == %p\n", element->Node.Next);
    ok(element->Node.Type == ElementType, "element->Node.Type == %d\n", element->Node.Type);

    if (element->FirstChild != NULL)
    {
        ok(element->FirstChild->Parent == element, "element->FirstChild->Parent = %p, expected %p\n", element->FirstChild->Parent, element);
        ok(element->FirstChild->Next == NULL, "element->FirstChild.Next == %p\n", element->FirstChild->Next);
        ok(element->FirstChild->Type == TextType, "element->FirstChild.Type == %d\n", element->Node.Type);
        ok(((WSDXML_TEXT *)element->FirstChild)->Text != NULL, "element->FirstChild.Text is null\n");
        ok(lstrcmpW(((WSDXML_TEXT *)element->FirstChild)->Text, text) == 0, "element->FirstChild->Text = %s, expected %s\n",
            wine_dbgstr_w(((WSDXML_TEXT *)element->FirstChild)->Text), wine_dbgstr_w(text));
    }

    WSDFreeLinkedMemory(element);
}

static void AddChild_tests(void)
{
    WSDXML_ELEMENT *parent, *child1, *child2;
    WSDXML_NAME parentName, child1Name, child2Name;
    WSDXML_NAMESPACE ns;
    WCHAR parentNameText[] = {'D','a','d',0};
    WCHAR child1NameText[] = {'T','i','m',0};
    WCHAR child2NameText[] = {'B','o','b',0};
    static const WCHAR uri[] = {'h','t','t','p',':','/','/','t','e','s','t','.','t','e','s','t','/',0};
    static const WCHAR prefix[] = {'t',0};
    HRESULT hr;

    /* Test invalid values */
    hr = WSDXMLAddChild(NULL, NULL);
    ok(hr == E_INVALIDARG, "WSDXMLAddChild failed with %08x\n", hr);

    hr = WSDXMLAddChild(parent, NULL);
    ok(hr == E_INVALIDARG, "WSDXMLAddChild failed with %08x\n", hr);

    hr = WSDXMLAddChild(NULL, child1);
    ok(hr == E_INVALIDARG, "WSDXMLAddChild failed with %08x\n", hr);

    /* Populate structures */
    ns.Uri = uri;
    ns.PreferredPrefix = prefix;

    parentName.LocalName = parentNameText;
    parentName.Space = &ns;

    child1Name.LocalName = child1NameText;
    child1Name.Space = &ns;

    child2Name.LocalName = child2NameText;
    child2Name.Space = &ns;

    /* Create some elements */
    hr = WSDXMLBuildAnyForSingleElement(&parentName, NULL, &parent);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&child1Name, child1NameText, &child1);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&child2Name, NULL, &child2);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    ok(parent->Node.Parent == NULL, "parent->Node.Parent == %p\n", parent->Node.Parent);
    ok(parent->FirstChild == NULL, "parent->FirstChild == %p\n", parent->FirstChild);
    ok(parent->Node.Next == NULL, "parent->Node.Next == %p\n", parent->Node.Next);
    ok(child1->Node.Parent == NULL, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->FirstChild != NULL, "child1->FirstChild == NULL\n");
    ok(child1->FirstChild->Type == TextType, "child1->FirstChild.Type == %d\n", child1->FirstChild->Type);
    ok(child1->Node.Next == NULL, "child1->Node.Next == %p\n", child1->Node.Next);

    /* Add the child to the parent */
    hr = WSDXMLAddChild(parent, child1);
    ok(hr == S_OK, "WSDXMLAddChild failed with %08x\n", hr);

    ok(parent->Node.Parent == NULL, "parent->Node.Parent == %p\n", parent->Node.Parent);
    ok(parent->FirstChild == (WSDXML_NODE *)child1, "parent->FirstChild == %p\n", parent->FirstChild);
    ok(parent->Node.Next == NULL, "parent->Node.Next == %p\n", parent->Node.Next);
    ok(child1->Node.Parent == parent, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->FirstChild != NULL, "child1->FirstChild == NULL\n");
    ok(child1->FirstChild->Type == TextType, "child1->FirstChild.Type == %d\n", child1->FirstChild->Type);
    ok(child1->Node.Next == NULL, "child1->Node.Next == %p\n", child1->Node.Next);

    /* Try to set child1 as the child of child2, which already has a parent */
    hr = WSDXMLAddChild(child2, child1);
    ok(hr == E_INVALIDARG, "WSDXMLAddChild failed with %08x\n", hr);

    /* Try to set child2 as the child of child1, which has a text node as a child */
    hr = WSDXMLAddChild(child2, child1);
    ok(hr == E_INVALIDARG, "WSDXMLAddChild failed with %08x\n", hr);

    /* Add child2 as a child of parent */
    hr = WSDXMLAddChild(parent, child2);
    ok(hr == S_OK, "WSDXMLAddChild failed with %08x\n", hr);

    ok(parent->Node.Parent == NULL, "parent->Node.Parent == %p\n", parent->Node.Parent);
    ok(parent->FirstChild == (WSDXML_NODE *)child1, "parent->FirstChild == %p\n", parent->FirstChild);
    ok(parent->Node.Next == NULL, "parent->Node.Next == %p\n", parent->Node.Next);
    ok(child1->Node.Parent == parent, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->FirstChild != NULL, "child1->FirstChild == NULL\n");
    ok(child1->FirstChild->Type == TextType, "child1->FirstChild.Type == %d\n", child1->FirstChild->Type);
    ok(child1->Node.Next == (WSDXML_NODE *)child2, "child1->Node.Next == %p\n", child1->Node.Next);
    ok(child2->Node.Parent == parent, "child2->Node.Parent == %p\n", child2->Node.Parent);
    ok(child2->FirstChild == NULL, "child2->FirstChild == %p\n", child2->FirstChild);
    ok(child2->Node.Next == NULL, "child2->Node.Next == %p\n", child2->Node.Next);

    WSDFreeLinkedMemory(parent);
}

static void AddSibling_tests(void)
{
    WSDXML_ELEMENT *parent, *child1, *child2, *child3;
    WSDXML_NAME parentName, child1Name, child2Name;
    WSDXML_NAMESPACE ns;
    WCHAR parentNameText[] = {'D','a','d',0};
    WCHAR child1NameText[] = {'T','i','m',0};
    WCHAR child2NameText[] = {'B','o','b',0};
    static const WCHAR uri[] = {'h','t','t','p',':','/','/','t','e','s','t','.','t','e','s','t','/',0};
    static const WCHAR prefix[] = {'t',0};
    HRESULT hr;

    /* Test invalid values */
    hr = WSDXMLAddSibling(NULL, NULL);
    ok(hr == E_INVALIDARG, "WSDXMLAddSibling failed with %08x\n", hr);

    hr = WSDXMLAddSibling(child1, NULL);
    ok(hr == E_INVALIDARG, "WSDXMLAddSibling failed with %08x\n", hr);

    hr = WSDXMLAddSibling(NULL, child2);
    ok(hr == E_INVALIDARG, "WSDXMLAddSibling failed with %08x\n", hr);

    /* Populate structures */
    ns.Uri = uri;
    ns.PreferredPrefix = prefix;

    parentName.LocalName = parentNameText;
    parentName.Space = &ns;

    child1Name.LocalName = child1NameText;
    child1Name.Space = &ns;

    child2Name.LocalName = child2NameText;
    child2Name.Space = &ns;

    /* Create some elements */
    hr = WSDXMLBuildAnyForSingleElement(&parentName, NULL, &parent);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&child1Name, child1NameText, &child1);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&child2Name, NULL, &child2);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    hr = WSDXMLBuildAnyForSingleElement(&child2Name, NULL, &child3);
    ok(hr == S_OK, "BuildAnyForSingleElement failed with %08x\n", hr);

    /* Add child1 to parent */
    hr = WSDXMLAddChild(parent, child1);
    ok(hr == S_OK, "WSDXMLAddChild failed with %08x\n", hr);

    ok(parent->Node.Parent == NULL, "parent->Node.Parent == %p\n", parent->Node.Parent);
    ok(parent->FirstChild == (WSDXML_NODE *)child1, "parent->FirstChild == %p\n", parent->FirstChild);
    ok(parent->Node.Next == NULL, "parent->Node.Next == %p\n", parent->Node.Next);
    ok(child1->Node.Parent == parent, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->FirstChild != NULL, "child1->FirstChild == NULL\n");
    ok(child1->FirstChild->Type == TextType, "child1->FirstChild.Type == %d\n", child1->FirstChild->Type);
    ok(child1->Node.Next == NULL, "child1->Node.Next == %p\n", child1->Node.Next);
    ok(child2->Node.Parent == NULL, "child2->Node.Parent == %p\n", child2->Node.Parent);
    ok(child2->FirstChild == NULL, "child2->FirstChild == %p\n", child2->FirstChild);
    ok(child2->Node.Next == NULL, "child2->Node.Next == %p\n", child2->Node.Next);

    /* Try to add child2 as sibling of child1 */
    hr = WSDXMLAddSibling(child1, child2);
    ok(hr == S_OK, "WSDXMLAddSibling failed with %08x\n", hr);

    ok(child1->Node.Parent == parent, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->Node.Next == (WSDXML_NODE *)child2, "child1->Node.Next == %p\n", child1->Node.Next);
    ok(child2->Node.Parent == parent, "child2->Node.Parent == %p\n", child2->Node.Parent);
    ok(child2->Node.Next == NULL, "child2->Node.Next == %p\n", child2->Node.Next);
    ok(parent->FirstChild == (WSDXML_NODE *)child1, "parent->FirstChild == %p\n", parent->FirstChild);

    /* Try to add child3 as sibling of child1 */
    hr = WSDXMLAddSibling(child1, child3);
    ok(hr == S_OK, "WSDXMLAddSibling failed with %08x\n", hr);

    ok(child1->Node.Parent == parent, "child1->Node.Parent == %p\n", child1->Node.Parent);
    ok(child1->Node.Next == (WSDXML_NODE *)child2, "child1->Node.Next == %p\n", child1->Node.Next);
    ok(child2->Node.Parent == parent, "child2->Node.Parent == %p\n", child2->Node.Parent);
    ok(child2->Node.Next == (WSDXML_NODE *)child3, "child2->Node.Next == %p\n", child2->Node.Next);
    ok(child3->Node.Parent == parent, "child2->Node.Parent == %p\n", child2->Node.Parent);
    ok(child3->Node.Next == NULL, "child2->Node.Next == %p\n", child2->Node.Next);
    ok(parent->FirstChild == (WSDXML_NODE *)child1, "parent->FirstChild == %p\n", parent->FirstChild);

    WSDFreeLinkedMemory(parent);
}

static void XMLContext_AddNamespace_tests(void)
{
    WCHAR ns1Uri[] = {'h','t','t','p',':','/','/','t','e','s','t','.','t','e','s','t',0};
    WCHAR ns2Uri[] = {'h','t','t','p',':','/','/','w','i','n','e','.','r','o','c','k','s',0};
    WCHAR ns3Uri[] = {'h','t','t','p',':','/','/','t','e','s','t','.','a','g','a','i','n',0};
    WCHAR ns4Uri[] = {'h','t','t','p',':','/','/','o','n','e','.','m','o','r','e',0};
    WCHAR prefix1[] = {'t','s','t',0};
    WCHAR prefix2[] = {'w','i','n','e',0};
    WCHAR unPrefix0[] = {'u','n','0',0};
    WCHAR unPrefix1[] = {'u','n','1',0};
    WCHAR unPrefix2[] = {'u','n','2',0};

    IWSDXMLContext *context;
    WSDXML_NAMESPACE *ns1 = NULL, *ns2 = NULL;
    HRESULT hr;

    hr = WSDXMLCreateContext(&context);
    ok(hr == S_OK, "WSDXMLCreateContext failed with %08x\n", hr);
    ok(context != NULL, "context == NULL\n");

    /* Test calling AddNamespace with invalid arguments */
    hr = IWSDXMLContext_AddNamespace(context, NULL, NULL, NULL);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    hr = IWSDXMLContext_AddNamespace(context, ns1Uri, NULL, NULL);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    hr = IWSDXMLContext_AddNamespace(context, NULL, prefix1, NULL);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    /* Test calling AddNamespace without the ppNamespace parameter */
    hr = IWSDXMLContext_AddNamespace(context, ns1Uri, prefix1, NULL);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Now retrieve the created namespace */
    hr = IWSDXMLContext_AddNamespace(context, ns1Uri, prefix1, &ns1);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Check the returned structure */
    todo_wine ok(ns1 != NULL, "ns1 == NULL\n");

    if (ns1 != NULL)
    {
        ok(lstrcmpW(ns1->Uri, ns1Uri) == 0, "URI returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns1->Uri));
        ok(lstrcmpW(ns1->PreferredPrefix, prefix1) == 0, "PreferredPrefix returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns1->PreferredPrefix));
        ok(ns1->Names == NULL, "Names array is not empty\n");
        ok(ns1->NamesCount == 0, "NamesCount is not 0 (value = %d)\n", ns1->NamesCount);
        ok(ns1->Uri != ns1Uri, "URI has not been cloned\n");
        ok(ns1->PreferredPrefix != prefix1, "URI has not been cloned\n");
    }

    /* Test calling AddNamespace with parameters that are too large */
    hr = IWSDXMLContext_AddNamespace(context, largeText, prefix2, &ns2);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    hr = IWSDXMLContext_AddNamespace(context, ns2Uri, largeText, &ns2);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    hr = IWSDXMLContext_AddNamespace(context, largeText, largeText, &ns2);
    todo_wine ok(hr == E_INVALIDARG, "AddNamespace failed with %08x\n", hr);

    /* Test calling AddNamespace with a conflicting prefix */
    hr = IWSDXMLContext_AddNamespace(context, ns2Uri, prefix1, &ns2);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Check the returned structure */
    todo_wine ok(ns2 != NULL, "ns2 == NULL\n");

    if (ns2 != NULL)
    {
        ok(lstrcmpW(ns2->Uri, ns2Uri) == 0, "URI returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->Uri));
        ok(lstrcmpW(ns2->PreferredPrefix, unPrefix0) == 0, "PreferredPrefix returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->PreferredPrefix));
        ok(ns2->Names == NULL, "Names array is not empty\n");
        ok(ns2->NamesCount == 0, "NamesCount is not 0 (value = %d)\n", ns2->NamesCount);
        ok(ns2->Uri != ns2Uri, "URI has not been cloned\n");
        ok(ns2->PreferredPrefix != prefix2, "URI has not been cloned\n");
    }

    WSDFreeLinkedMemory(ns1);
    WSDFreeLinkedMemory(ns2);

    /* Try explicitly creating a prefix called 'un1' */
    hr = IWSDXMLContext_AddNamespace(context, ns4Uri, unPrefix1, &ns2);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Check the returned structure */
    todo_wine ok(ns2 != NULL, "ns2 == NULL\n");

    if (ns2 != NULL)
    {
        ok(lstrcmpW(ns2->PreferredPrefix, unPrefix1) == 0, "PreferredPrefix returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->PreferredPrefix));
    }

    WSDFreeLinkedMemory(ns2);

    /* Test with one more conflicting prefix */
    hr = IWSDXMLContext_AddNamespace(context, ns3Uri, prefix1, &ns2);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Check the returned structure */
    todo_wine ok(ns2 != NULL, "ns2 == NULL\n");

    if (ns2 != NULL)
    {
        ok(lstrcmpW(ns2->PreferredPrefix, unPrefix2) == 0, "PreferredPrefix returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->PreferredPrefix));
    }

    WSDFreeLinkedMemory(ns2);

    /* Try renaming a prefix */
    hr = IWSDXMLContext_AddNamespace(context, ns3Uri, prefix2, &ns2);
    todo_wine ok(hr == S_OK, "AddNamespace failed with %08x\n", hr);

    /* Check the returned structure */
    todo_wine ok(ns2 != NULL, "ns2 == NULL\n");

    if (ns2 != NULL)
    {
        ok(lstrcmpW(ns2->Uri, ns3Uri) == 0, "Uri returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->Uri));
        ok(lstrcmpW(ns2->PreferredPrefix, prefix2) == 0, "PreferredPrefix returned by AddNamespace is not as expected (%s)\n", wine_dbgstr_w(ns2->PreferredPrefix));
    }

    WSDFreeLinkedMemory(ns2);

    IWSDXMLContext_Release(context);
}

START_TEST(xml)
{
    /* Allocate a large text buffer for use in tests */
    largeText = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, largeTextSize + sizeof(WCHAR));
    memset(largeText, 'a', largeTextSize);

    BuildAnyForSingleElement_tests();
    AddChild_tests();
    AddSibling_tests();

    XMLContext_AddNamespace_tests();

    HeapFree(GetProcessHeap(), 0, largeText);
}
