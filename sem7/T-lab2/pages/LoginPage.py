from pages._xpath import Tag, Param, Condition
import pages


class LoginPage(pages.BasePage):
    def __init__(self, driver=None, *args, **kwargs):
        super(LoginPage, self).__init__(driver, None, "login", *args, **kwargs)

    def login(self, login: str, password: str) -> pages.MainPage:
        self._fill_form(("login", login), ("password", password))
        self._find_visible_enabled(Tag.Input, (Param.Name, Condition.Equals, "commit")).click()
        return pages.MainPage(self._driver, login)
